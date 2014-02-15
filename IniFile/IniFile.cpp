#include "IniFile.h"
#include <utility>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <boost/make_shared.hpp>
#include <boost/thread.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/throw_exception.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/algorithm/string/trim.hpp>
#include "float_to_string.hpp"
//-----------------------------------------------------------------------------
namespace hgl {
//-----------------------------------------------------------------------------
namespace ini_file {
//-----------------------------------------------------------------------------
//in-place convert string into sensible for due to bug in std::getline()...
static struct _TidyStringAfterGetline
{
	void operator() (std::string& Line) const
	{
		size_t Pos = Line.find_first_of('\0');

		if (Pos < std::string::npos)
			Line.erase(Line.begin() + Pos, Line.end());
	}
} TidyStringAfterGetline;
//-----------------------------------------------------------------------------
//split string exception class...
class ESplitStringBadDelimError : public std::runtime_error
{
public:
	ESplitStringBadDelimError(const std::string& Message)
		: std::runtime_error(Message)
	{ }
};
//-----------------------------------------------------------------------------
//split a std::string into two halves either side of a delimiter, throws
//ESplitStringBadDelimError if Delim not found...
static struct _SplitString
{
	void operator()(std::string& SubStr1,
					std::string& SubStr2,
					const std::string& ToSplit,
					const char Delim,
					const bool Trimmed)
	{
		//find pos of delim...
		size_t Pos = ToSplit.find_first_of(Delim);
		//if delim found split string either side of it...
		if (Pos < ToSplit.size())
		{
			if (Trimmed)
			{
				SubStr1 = ToSplit.substr(0, Pos);
				boost::trim(SubStr1);

				SubStr2 = ToSplit.substr(Pos + 1, ToSplit.size() - Pos);
				boost::trim(SubStr2);
			}
			else
			{
            	SubStr1 = ToSplit.substr(0, Pos);
				SubStr2 = ToSplit.substr(Pos + 1, ToSplit.size() - Pos);
			}
		}
		else
		{
			BOOST_THROW_EXCEPTION(ESplitStringBadDelimError("Cannot find delimter in string: " + ToSplit));
		}
	}
} SplitString;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//object to hold key and value info or a comment...
class KeyValuePair
{
public:
    //default constructor...
	KeyValuePair()
        : m_Comment(false)
	{ }

    //initialise constructor for true key-value pair...
	KeyValuePair(const std::string& Key, 
		         const std::string& Value) 
	    : m_Key(Key), m_Value(Value), 
          m_Comment(false)
	{ }

    //initialise constructor for holding a comment...
	KeyValuePair(const std::string& Comment) 
	    : m_Key(Comment), m_Comment(true)
	{ }

    //copy constructor...
	KeyValuePair(const KeyValuePair& RHS)
		 : m_Key(RHS.m_Key), m_Value(RHS.m_Value),
           m_Comment(RHS.m_Comment)
	{ }

	virtual ~KeyValuePair() { }

	//handle copy assignment...
	KeyValuePair& operator=(const KeyValuePair& RHS)
	{
		if (this != &RHS)
		{
			m_Key = RHS.m_Key;
			m_Value = RHS.m_Value;
			m_Comment = RHS.m_Comment;
		}

		return *this;
	}

	//equality operator (against another KeyValuePair)...
	bool operator==(const KeyValuePair& RHS) const
	{
		bool Equals = this == &RHS ? true
					  : m_Key.compare(RHS.m_Key) == 0;
		return Equals;
	}

	//equality operator (against a key name)...
    bool operator==(const std::string& Key) const
	{
		return m_Key.compare(Key) == 0;
	}

	//less than operator (comparing key names)
	bool operator<(const KeyValuePair& RHS) const
	{
		return m_Key.compare(RHS.m_Key) < 0;
	}

	//get the key name...
    const std::string& Key() const
    {
        return m_Key;
    }

	//get the value...
    const std::string& Value() const
    {
        return m_Value;
    }

	//is this a comment line...
    bool IsComment() const
    {
        return m_Comment;
    }

private:
	//private data...
	std::string m_Key;
    std::string m_Value;
    bool m_Comment;

	//friend the appropriate operator<< for streaming...
	friend std::ostream& operator<<(std::ostream& os, const KeyValuePair& KVP);
};
//-----------------------------------------------------------------------------
//output stream operator for key value pair when writing back to ini file...
std::ostream& operator<<(std::ostream& os, const KeyValuePair& KVP)
{
	if (KVP.m_Comment)
		os << KVP.m_Key;
	else
		os << KVP.m_Key << "=" << KVP.m_Value;

	return os;
}
//-----------------------------------------------------------------------------
//useful typedefs...
typedef boost::shared_ptr< KeyValuePair >           key_val_pair_ptr;
typedef std::vector< key_val_pair_ptr >				key_val_pair_vec;
typedef key_val_pair_vec::const_iterator			key_val_pair_vec_const_iter;
typedef key_val_pair_vec::iterator					key_val_pair_vec_iter;
//-----------------------------------------------------------------------------
//object to hold ini file section information...
class IniSection
{
public:
	//default constructor...
	IniSection() { }

	//initialise constructor...
	IniSection(const std::string& SectionName)
		: m_SectionName(SectionName)
	{ }

	//copy IniSection...
	IniSection(const IniSection& RHS)
		 : m_SectionName(RHS.m_SectionName),
		   m_KVPs(RHS.m_KVPs)
	{ }

	//virtual destructor...
	virtual ~IniSection() {}

	//handle copy assignment...
	IniSection& operator=(const IniSection& RHS)
	{
		if (this != &RHS)
		{
			m_SectionName = RHS.m_SectionName;
			m_KVPs = RHS.m_KVPs;
		}

		return *this;
	}

	//equality operator (against IniFile object)...
	bool operator==(const IniSection& RHS) const
	{
		bool Equals = this == &RHS ? true
					  : m_SectionName.compare(RHS.m_SectionName) == 0;
		return Equals;
	}

	//comparison operator (against section name)...
	bool operator==(const std::string& SectionName) const
	{
		return m_SectionName.compare(SectionName) == 0;
	}

	//less than operator (comparing section names)
	bool operator<(const IniSection& RHS) const
	{
		return m_SectionName.compare(RHS.m_SectionName) < 0;
	}

	//get an const iterator to a key value pair...
	bool FindKeyInSection(const std::string& Key,
						  key_val_pair_vec_const_iter& KVPIter) const
	{
		KVPIter = m_KVPs.begin();

		while(KVPIter != m_KVPs.end())
		{
			if ((*KVPIter)->Key() == Key)
				break;

			++KVPIter;
		}

		return KVPIter != m_KVPs.end();
	}

	//get a non-const iterator to a key value pair...
	bool FindKeyInSection(const std::string& Key,
						  key_val_pair_vec_iter& KVPIter)
	{
		KVPIter = m_KVPs.begin();

		while(KVPIter != m_KVPs.end())
		{
			if ((*KVPIter)->Key() == Key)
				break;

			++KVPIter;
		}

		return KVPIter != m_KVPs.end();
	}

	//get a vector of key names...
	void GetKeys(std::vector< std::string >& Keys) const
	{
		key_val_pair_vec_const_iter KeyIter(m_KVPs.begin());

		while(KeyIter != m_KVPs.end())
		{
			if (!(*KeyIter)->IsComment())
				Keys.push_back((*KeyIter)->Key());

			++KeyIter;
		}
	}

	//get a vector of values...
	void GetValues(std::vector< std::string >& Values) const
	{
		key_val_pair_vec_const_iter KeyIter(m_KVPs.begin());

		while(KeyIter != m_KVPs.end())
		{
			if (!(*KeyIter)->IsComment())
				Values.push_back((*KeyIter)->Value());

			++KeyIter;
		}
	}

	//add new key value pair (if not a duplicate)...
	void AddKeyValuePair(key_val_pair_ptr& KVP)
	{
		key_val_pair_vec_const_iter kvpIter;

		if (KVP->IsComment() || !FindKeyInSection(KVP->Key(), kvpIter))
			m_KVPs.push_back(KVP);
	}

	//delete a key value pair...
	void DeleteKeyValuePair(const std::string& Key)
	{
		key_val_pair_vec_iter KVPIter;

		if (FindKeyInSection(Key, KVPIter))
			m_KVPs.erase(KVPIter);
	}

	//delete all key value pairs...
	void DeleteKeyValuePairs()
	{
		m_KVPs.clear();
	}

	//get the section name...
	const std::string& SectionName() const
	{
		return m_SectionName;
	}

	//get a const ref to key value pair vector...
	const key_val_pair_vec& KeyValuePairs() const
	{
		return m_KVPs;
	}

private:
	//private data...
	std::string m_SectionName;
	key_val_pair_vec m_KVPs;

	//friend the appropriate operator<< for streaming...
	friend std::ostream& operator<<(std::ostream& os, const IniSection& Section);
};

//-----------------------------------------------------------------------------
//output stream operator for section for writing back to ini file...
std::ostream& operator<<(std::ostream& os, const IniSection& Section)
{
	//add section heading if this isn't a "special" entry
	//for a pure comment block...
	if (Section.m_SectionName.compare("!!! THIS IS A COMMENT !!!") != 0)
		os << "[" << Section.m_SectionName << "]" << std::endl;

	//now add key value pairs to file by invoking operator<<
	//on each key-value pair...
	key_val_pair_vec_const_iter KeyIter(Section.m_KVPs.begin());

	while(KeyIter != Section.m_KVPs.end())
	{
		os << (*(*KeyIter));
		++KeyIter;

		if (KeyIter != Section.m_KVPs.end())
			os << std::endl;
	}

	return os;
}

//-----------------------------------------------------------------------------
//object to hold all our private data...
class IniFile::IniFilePImpl
{
private:
	//useful typedefs...
	typedef boost::shared_ptr< IniSection >   section_ptr;
	typedef std::vector< section_ptr >		  section_vector;
	typedef section_vector::const_iterator	  section_const_iter;
	typedef section_vector::iterator		  section_iter;

public:
	//initialisation constructor...
	IniFilePImpl(const std::string& IniFilePath)
		: m_IniFilePath(IniFilePath)
	{
        //let's read the ini file into memory...
		ParseIniFile();
	}

	IniFilePImpl(const std::string& IniFilePath, IniFile::DoNotReadIni_t)
		: m_IniFilePath(IniFilePath)
	{
    }

	void ReadFile()
    {
        EraseSections();
		ParseIniFile();
	}

	//virtual destructor...
    virtual ~IniFilePImpl() 
	{
		boost::lock_guard< boost::mutex > Lock(m_Mutex);
    }

	//call to update file on disk...
	void UpdateFile() const
    {
	    boost::lock_guard< boost::mutex > Lock(m_Mutex);
        bool Throw = false;

        try
	    {
		    //create string stream and fill this with our
		    //formatted output...
		    std::stringstream sstream;

		    //if stream is open...
		    if (sstream.good())
            {
			    section_const_iter SecIter(m_Sections.begin());

				//output sections to file invoking operator<<
				//on each section object...
			    while(SecIter != m_Sections.end())
				{
					sstream << (*(*SecIter));
					++SecIter;

				    if (SecIter != m_Sections.end())
					    sstream << std::endl << std::endl;
			    }

			    //only save to disk now, this way we to minimise the amount
			    //of time we have the file open for...
			    if (!SaveStringStreamToFile(sstream))
				    Throw = true;
            }
            else
		    {
			    Throw = true;
            }
        }
        catch(...)
        {
            Throw = true;
        }

        if (Throw)
            BOOST_THROW_EXCEPTION(EIniFileSaveError("Failed to save ini file."));
    }

	//get section names...
    void ReadSections(std::vector< std::string >& Sections) const
    {
	    Sections.clear();
	
		boost::lock_guard< boost::mutex > Lock(m_Mutex);
		section_const_iter SecIter(m_Sections.begin());

	    while(SecIter != m_Sections.end())
	    {
			if ((*SecIter)->SectionName().compare("!!! THIS IS A COMMENT !!!") != 0)
				Sections.push_back((*SecIter)->SectionName());

		    ++SecIter;
		}
    }

	//get key names for section...
    void ReadSection(const std::string& Section, 
					 std::vector< std::string >& Keys) const
    {
		Keys.clear();
        if (Section.compare("") == 0) return;

	    boost::lock_guard< boost::mutex > Lock(m_Mutex);
		section_const_iter SecIter;

		if (FindSectionInVector(Section, SecIter))
			(*SecIter)->GetKeys(Keys);
    }

	//get values for section...
    void ReadSectionValues(const std::string& Section, 
						   std::vector< std::string >& Values) const
    {
	    Values.clear();
        if (Section.compare("") == 0) return;

	    boost::lock_guard< boost::mutex > Lock(m_Mutex);
		section_const_iter SecIter;

		if (FindSectionInVector(Section, SecIter))
			(*SecIter)->GetValues(Values);
    }

	//Does a section exist?
    bool SectionExists(const std::string& Section) const
    {
	    boost::lock_guard< boost::mutex > Lock(m_Mutex);
		section_const_iter SecIter;
		return FindSectionInVector(Section, SecIter);
    }

	//Does a key value pair exist for a section?
    bool ValueExists(const std::string& Section,
				     const std::string& Key) const
    {
	    boost::lock_guard< boost::mutex > Lock(m_Mutex);
	    key_val_pair_vec_const_iter KeyIter;
	    return FindKeyValuePairForSectionAndKey(Section, Key, KeyIter);
    }

	//erase a section...
    void EraseSection(const std::string& Section)
    {
        if (Section.compare("") == 0) return;

	    boost::lock_guard< boost::mutex > Lock(m_Mutex);
        section_iter SecIter;

		if (FindSectionInVector(Section, SecIter))
			m_Sections.erase(SecIter);
    }

	//erase all sections...
	void EraseSections()
    {
        boost::lock_guard< boost::mutex > Lock(m_Mutex);
        m_Sections.clear();
    }

	//delete a key value pair in a section...
    void DeleteKey(const std::string& Section,
				   const std::string& Key)
    {
        boost::lock_guard< boost::mutex > Lock(m_Mutex);
        section_iter SecIter;

		if (FindSectionInVector(Section, SecIter))
			(*SecIter)->DeleteKeyValuePair(Key);
    }

	//delete all key value pairs in a section...
    void DeleteKeys(const std::string& Section)
    {
        boost::lock_guard< boost::mutex > Lock(m_Mutex);
        section_iter SecIter;

		if (FindSectionInVector(Section, SecIter))
			(*SecIter)->DeleteKeyValuePairs();
    }

	//read a value as a boolean...
    bool ReadBool(const std::string& Section,
		          const std::string& Key,
				  const bool DefaultValue) const
    {
        boost::lock_guard< boost::mutex > Lock(m_Mutex);
        const int TempDefaultValue = DefaultValue ? 1 : 0;    
        return GetValueForSectionAndKey(Section, Key, TempDefaultValue) == 1;
    }

	//read a value as an integer number...
    int ReadInteger(const std::string& Section,
		            const std::string& Key,
					const int DefaultValue) const
    {
	    boost::lock_guard< boost::mutex > Lock(m_Mutex);
        return GetValueForSectionAndKey(Section, Key, DefaultValue);
    }

	//read a value as a double precision floating point number...
    double ReadFloat(const std::string& Section,
		             const std::string& Key,
					 const double DefaultValue) const
    {
	    boost::lock_guard< boost::mutex > Lock(m_Mutex);
        return GetValueForSectionAndKey(Section, Key, DefaultValue);
    }

	//read a value as a string...
    std::string ReadString(const std::string& Section,
		                   const std::string& Key,
						   const std::string& DefaultValue) const
    {
	    boost::lock_guard< boost::mutex > Lock(m_Mutex);
        return GetValueForSectionAndKey(Section, Key, DefaultValue);
    }

	//write a value as a boolean...
    void WriteBool(const std::string& Section,
		           const std::string& Key,
				   const bool Value)
    {
	    boost::lock_guard< boost::mutex > Lock(m_Mutex);
        const int TempValue = Value ? 1 : 0;
        SetValueForSectionAndKey(Section, Key, TempValue);
    }

	//write a value as an integer value...
    void WriteInteger(const std::string& Section,
		              const std::string& Key,
					  const int Value)
    {
	    boost::lock_guard< boost::mutex > Lock(m_Mutex);
        SetValueForSectionAndKey(Section, Key, Value);
    }

	//write a value as a double precision floating point value...
    void WriteFloat(const std::string& Section,
		            const std::string& Key,
					const double Value)
    {
		boost::lock_guard< boost::mutex > Lock(m_Mutex);
		//avoid rounding issue with lexical cast...
		SetValueForSectionAndKey(Section, Key, std::string(float_to_string(Value)));
    }

	//write a value as a string...
    void WriteString(const std::string& Section,
					 const std::string& Key,
				     const std::string& Value)
    {
	    boost::lock_guard< boost::mutex > Lock(m_Mutex);
        SetValueForSectionAndKey(Section, Key, Value);
    }

private:
	//private data...
    mutable boost::mutex m_Mutex;
	const std::string m_IniFilePath;
	section_vector m_Sections;

	//load the ini file into a stream object...
	bool LoadFileIntoStringStream(std::stringstream& sstream) const
    {
	    bool Success = false;

	    //create file stream...
	    std::ifstream infile(m_IniFilePath.c_str());

	    if (infile.is_open() && infile.good())
	    {
            //compute length of file...
		    infile.seekg (0, std::ios::end);
            const std::streamoff length = infile.tellg();
		    infile.seekg (0, std::ios::beg);

            if ((length > 0) && (length <= std::numeric_limits<size_t>::max()))
			{
				std::string fileasstring;

				//reduce scope of large vector...
				{
					//load file into vector...
					std::vector<char> filebuf(boost::numeric_cast<size_t>(length));
					infile.read(&filebuf[0], length);
					//close file stream...
					infile.close();
					//create string of file contents...
					fileasstring.assign(filebuf.begin(), filebuf.end());
				}

		        //fill string stream from temp string...
		        sstream << fileasstring;
		        //set success...
		        Success = true;
            }
            else
            {
                //close file stream...
		        infile.close();
            }
		}
		else
		{
			//set success as we have no ini file but don't want to
			//stop ourselves creating one...
			Success = true;
		}

	    return Success;
    }

	//parse the stream object containing the ini file...
	void ParseIniFile()
    {
	    bool Throw = false;

        try
        {
		    //let's be a bit clever, we want to mimise how long we
		    //have the file open so we'll buffer it up into a
		    //stringstream and work on that instead...
		    std::stringstream sstream;

		    //is stream open and string stream filled ok...
		    if (LoadFileIntoStringStream(sstream))
		    {
			    //need to keep current section persistent
			    //during while loop...
			    std::string CurrentSection;
                std::string PreviousSection;

                //get each line until end of stream or an error occurs...
			    while (sstream.good())
			    {
				    std::string Line;
				    std::getline(sstream, Line);

				    //filter out empty lines (there is a known bug in std::getline which means it
				    //can add extra '\0' and junk chars to a string so although doing c_str()
				    //gives a correct null terminated c-styled string the size() of the std::string
				    //will be more than it should be so std::string::operator= will fail if compared
					//to a string literal that matches the c_str() due to the size being wrong....
					TidyStringAfterGetline(Line);
					boost::trim(Line);

					if (Line != "")
                    {
					    //is this a new section?
					    if ((Line.size() >= 2) && (Line[0] == '[')
                            && (Line[Line.size() - 1] == ']'))
					    {
                            //remember previous section...
                            PreviousSection = CurrentSection;

						    //store section...
							CurrentSection.assign(Line.begin() + 1, Line.end() - 1);

							//trim spaces from beginning and end...
							boost::trim(CurrentSection);

							//add new section to vector...
							if (CurrentSection != "")
								m_Sections.push_back(boost::make_shared< IniSection >(CurrentSection));
                        }
                        //is this a key-value pair or comment within a section...
						else if (CurrentSection != "")
					    {
							//if this is a comment line then add comment line
							//to current section...
							if (Line[0] == ';')
							{
								AddKeyValuePairToSection(CurrentSection, boost::make_shared< KeyValuePair >(Line));
                            }
							//else if we're a potential normal line in a regular section...
                            else if (CurrentSection.compare("!!! THIS IS A COMMENT !!!") != 0)
                            {
						        try
						        {
                                    std::string Key, Value;

							        //split line into key and value either side of '=' delimiter...
							        SplitString(Key, Value, Line, '=', true);  

							        //add key-value pair to current section...
									if (Key != "")
										AddKeyValuePairToSection(CurrentSection, boost::make_shared< KeyValuePair >(Key, Value));
						        }
								catch(ESplitStringBadDelimError& E)
						        {
							        //split string couldn't find '=' delimiter so skip
							        //this entry in ini file...
							        E;//stop compiler warning in VC++
						        }
						        catch(...)
						        {
							        //some unexpected error so break and throw...
							        Throw = true;
							        break;
						        }
                            }
                        }
                        //else if we are a comment at start of file...
                        else if ((PreviousSection.compare("") == 0) 
                                 && (Line[0] == ';'))
                        {
                            //set current section to special section...
                            CurrentSection = "!!! THIS IS A COMMENT !!!";

							//create section and add comment line to it...
							AddKeyValuePairToSection(CurrentSection, boost::make_shared< KeyValuePair >(Line));
						}
                        //else if we are a comment stranded in a bad section...
                        else if ((PreviousSection.compare("") != 0) 
                                 && (Line[0] == ';'))
                        {
                            //add comment line to special section...                            
							AddKeyValuePairToSection(PreviousSection, boost::make_shared< KeyValuePair >(Line));
						}
                    }
			    }

                //if not equal to end of file there must have been an error...
				if (!sstream.eof() && !Throw)
					Throw = true;
		    }
        }
        catch(...)
        {
            Throw = true;
        }

        if (Throw)
            BOOST_THROW_EXCEPTION(EIniFileParserError("Failed to parse ini file."));
    }

	//save the output stream object to disk...
    bool SaveStringStreamToFile(const std::stringstream& sstream) const
    {
	    bool Success;

	    //only create file stream now, this way we to minimise the amount
	    //of time we have the file open for...
	    std::ofstream outfile(m_IniFilePath.c_str(), std::ios::trunc);

	    //if stream is open...
	    if (outfile.is_open() && outfile.good())
	    {
		    //write entire contents of stringstream to the
		    //file in one go...
		    outfile << sstream.str();

		    //flush and close stream as we're done...
		    outfile.flush();
		    outfile.close();

		    //set success...
		    Success = true;
	    }
	    else
	    {
		    Success = false;
	    }

	    return Success;
	}

	//add a key value pair to a section guarding against duplicates...
	void AddKeyValuePairToSection(const std::string& Section,
								  key_val_pair_ptr&& KVP)
	{
		section_iter SecIter;

		//if section exists add to section...
		if (FindSectionInVector(Section, SecIter))
		{
			(*SecIter)->AddKeyValuePair(KVP);
		}
		//else create new section and add key value pair...
		else
		{
			section_ptr NewSection(boost::make_shared<IniSection>(Section));
			NewSection->AddKeyValuePair(KVP);
			m_Sections.push_back(NewSection);
		}
	}

	//get a const iterator to a section...
	bool FindSectionInVector(const std::string& Section,
							 section_const_iter& SecIter) const
	{
		SecIter = m_Sections.begin();

		while(SecIter != m_Sections.end())
		{
			if ((*SecIter)->SectionName() == Section)
				break;

			++SecIter;
		}

		return SecIter != m_Sections.end();
	}

	//get a non-const iterator to a section...
	bool FindSectionInVector(const std::string& Section,
							 section_iter& SecIter)
	{
		SecIter = m_Sections.begin();

		while(SecIter != m_Sections.end())
		{
			if ((*SecIter)->SectionName() == Section)
				break;

			++SecIter;
		}

		return SecIter != m_Sections.end();
	}

	//get a const iterator to a key value pair in a section...
	bool FindKeyValuePairForSectionAndKey(const std::string& Section,
										  const std::string& Key,
										  key_val_pair_vec_const_iter& KeyIter) const
	{
		section_const_iter SecIter;

		if (FindSectionInVector(Section, SecIter))
			return (*SecIter)->FindKeyInSection(Key, KeyIter);
		else
			return false;
	}

	//get a value for a key in a section casting to the
	//correct type if possible...
    template <typename T>
    T GetValueForSectionAndKey(const std::string& Section,
							   const std::string& Key,
							   const T& TDefaultValue) const
    {
        T Value;
		key_val_pair_vec_const_iter KeyIter;

		if (FindKeyValuePairForSectionAndKey(Section, Key, KeyIter))
		{
            try
            {
				Value = boost::lexical_cast<T>((*KeyIter)->Value());
            }
            catch(...)
            {  
                //we'll just return the default value...
				Value = TDefaultValue;
            }
        }
		else
		{
			//we'll just return the default value...
			Value = TDefaultValue;
		}

	    return Value;
    }

	//set a value for a key in a section casting to the
	//correct type if possible...
    template <typename T>
    void SetValueForSectionAndKey(const std::string& Section,
								  const std::string& Key,
                                  const T& TValue)
    {
		//make sure section isn't silly...
        if (Section.compare("") == 0)
            BOOST_THROW_EXCEPTION(EIniFileInvalidSectionError("Invalid Section, must be non-empty."));

		//make sure key isn't silly...
        if (Key.compare("") == 0)
            BOOST_THROW_EXCEPTION(EIniFileInvalidKeyError("Invalid Key, must be non-empty."));

		//now we can update or add as appropriate...
        try
		{
			//create a key-value pair...
			key_val_pair_ptr KVP(boost::make_shared< KeyValuePair >(Key, boost::lexical_cast<std::string>(TValue)));
			section_iter SecIter;

			if (FindSectionInVector(Section, SecIter))
			{
				key_val_pair_vec_iter KeyIter;

				//update existing key-value pair with new value...
				if ((*SecIter)->FindKeyInSection(Key, KeyIter))
					(*KeyIter) = KVP;
				//key not found so add new key-value pair...
				else
					(*SecIter)->AddKeyValuePair(KVP);
            }
            else
            {
				//section doesn't exist so add new section and add the
				//key-value pair to it...
				section_ptr NewSection(boost::make_shared<IniSection>(Section));
				NewSection->AddKeyValuePair(KVP);
				m_Sections.push_back(NewSection);
            }
        }
        catch(...)
        {
            BOOST_THROW_EXCEPTION(EIniFileDataConvertError("Invalid value type, cannot convert to std::string."));
        }
    }
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//constructor, throws standard exceptions and EIniFileParserError...
IniFile::IniFile(const std::string& IniFilePath)
	: m_PImpl(boost::make_shared<IniFile::IniFilePImpl>(IniFilePath))
{
}

IniFile::IniFile(const std::string& IniFilePath, DoNotReadIni_t doNotReadIni)
	: m_PImpl(boost::make_shared<IniFile::IniFilePImpl>(IniFilePath, doNotReadIni))
{
}

IniFile::~IniFile()
{
}

void IniFile::ReadFile() const
{
	m_PImpl->ReadFile();
}


void IniFile::UpdateFile() const
{
    m_PImpl->UpdateFile();
}

void IniFile::ReadSections(std::vector< std::string >& Sections) const
{
    m_PImpl->ReadSections(Sections);
}

void IniFile::ReadSection(const std::string& Section,
		            std::vector< std::string >& Keys) const
{
    m_PImpl->ReadSection(Section, Keys);
}

void IniFile::ReadSectionValues(const std::string& Section,
						        std::vector< std::string >& Values) const
{
    m_PImpl->ReadSectionValues(Section, Values);
}

bool IniFile::SectionExists(const std::string& Section) const
{
    return m_PImpl->SectionExists(Section);
}

bool IniFile::ValueExists(const std::string& Section,
					      const std::string& Key) const
{
    return m_PImpl->ValueExists(Section, Key);
}

bool IniFile::ReadBool(const std::string& Section,
		               const std::string& Key,
				       const bool DefaultValue) const
{
    return m_PImpl->ReadBool(Section, Key, DefaultValue);
}

int IniFile::ReadInteger(const std::string& Section,
		                 const std::string& Key,
				         const int DefaultValue) const
{
    return m_PImpl->ReadInteger(Section, Key, DefaultValue);
}

double IniFile::ReadFloat(const std::string& Section,
		                  const std::string& Key,
					      const double DefaultValue) const
{
    return m_PImpl->ReadFloat(Section, Key, DefaultValue);
}

std::string IniFile::ReadString(const std::string& Section,
		                        const std::string& Key,
						        const std::string& DefaultValue) const
{
    return m_PImpl->ReadString(Section, Key, DefaultValue);
}

void IniFile::WriteBool(const std::string& Section,
		                const std::string& Key,
				        const bool Value)
{
    m_PImpl->WriteBool(Section, Key, Value);
}

void IniFile::WriteInteger(const std::string& Section,
		                   const std::string& Key,
					       const int Value)
{
    m_PImpl->WriteInteger(Section, Key, Value);
}

void IniFile::WriteFloat(const std::string& Section,
		                 const std::string& Key,
				         const double Value)
{
    m_PImpl->WriteFloat(Section, Key, Value);
}

void IniFile::WriteString(const std::string& Section,
		                  const std::string& Key,
				          const std::string& Value)
{
    m_PImpl->WriteString(Section, Key, Value);
}

void IniFile::EraseSection(const std::string& Section)
{
    m_PImpl->EraseSection(Section);
}

void IniFile::EraseSections()
{
    m_PImpl->EraseSections();
}

void IniFile::DeleteKey(const std::string& Section,
                        const std::string& Key)
{
    m_PImpl->DeleteKey(Section, Key);
}

void IniFile::DeleteKeys(const std::string& Section)
{
    m_PImpl->DeleteKeys(Section);
}
//-----------------------------------------------------------------------------
} //namespace ini_file
//-----------------------------------------------------------------------------
} //namespace hgl
//-----------------------------------------------------------------------------
