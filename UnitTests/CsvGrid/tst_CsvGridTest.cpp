#include <QString>
#include <QtTest>
#include "../../CsvGrid.hpp"
#include <limits>
#include <fstream>
#include "boost/predef.h"

using namespace core_lib::csv_grid;

class CsvGridTest : public QObject
{
    Q_OBJECT

public:
    CsvGridTest();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void Case1_xCsvGridColOutOfRangeError_1();
    void Case2_xCsvGridColOutOfRangeError_2();
    void Case3_xCsvGridDimensionError_1();
    void Case4_xCsvGridDimensionError_2();
    void Case5_xCsvGridRowOutOfRangeError_1();
    void Case6_xCsvGridRowOutOfRangeError_2();
    void Case7_xCsvGridCreateFileStreamError_1();
    void Case8_xCsvGridCreateFileStreamError_2();
    void Case9_Cell_DefaultConstructor();
    void Case10_Cell_StringConstructor();
    void Case11_Cell_CopyConstructor();
    void Case12_Cell_MoveConstructor();
    void Case13_Cell_Int32Constructor();
    void Case14_Cell_Int64Constructor();
    void Case15_Cell_DoubleConstructor();
    void Case16_Cell_CopyAssignment();
    void Case17_Cell_MoveAssignment();
    void Case18_Cell_StringAssignment();
    void Case19_Cell_Int32Assignment();
    void Case20_Cell_Int64Assignment();
    void Case21_Cell_DoubleAssignment();
    void Case22_Cell_Int32ConvertException();
    void Case23_Cell_Int64ConvertException();
    void Case24_Cell_DoubleConvertException();
    void Case25_Cell_ToInt32Def_1();
    void Case26_Cell_ToInt32Def_2();
    void Case27_Cell_ToInt64Def_1();
    void Case28_Cell_ToInt64Def_2();
    void Case29_Cell_ToDoubleDef_1();
    void Case30_Cell_ToDoubleDef_2();
    void Case31_Row_DefaultConstructor();
    void Case32_Row_CopyConstructor();
    void Case33_Row_MoveConstructor();
    void Case34_Row_InitializingConstructor();
    void Case35_Row_InitializerListCellConstructor();
    void Case36_Row_InitializerListStringConstructor();
    void Case37_Row_InitializerListInt32Constructor();
    void Case38_Row_InitializerListInt64Constructor();
    void Case39_Row_InitializerListDoubleConstructor();
    void Case40_Row_CopyAssignment();
    void Case41_Row_MoveAssignment();
    void Case42_Row_SubscriptOperator();
    void Case43_Row_AddColumnAsString();
    void Case44_Row_AddColumnAsInt32();
    void Case45_Row_AddColumnAsInt64();
    void Case46_Row_AddColumnAsDouble();
    void Case47_Row_InsertColumnAsString();
    void Case48_Row_InsertColumnAsInt32();
    void Case49_Row_InsertColumnAsInt64();
    void Case50_Row_InsertColumnAsDouble();
    void Case51_Row_ClearCells();
    void Case52_Row_ResetRow();
    void Case53_CsvGrid_DefaultConstructor();
    void Case54_CsvGrid_InitializingConstructor_1();
    void Case55_CsvGrid_InitializingConstructor_2();
    void Case56_CsvGrid_InitializerListConstructor();
    void Case57_CsvGrid_CopyConstructor();
    void Case58_CsvGrid_MoveConstructor();
    void Case59_CsvGrid_FileConstructor_SimpleCells();
    void Case60_CsvGrid_FileConstructor_SimpleCells_Benchmark_1000by130Cells();
    void Case61_CsvGrid_FileConstructor_DoubleQuotedCells();
    void Case62_CsvGrid_FileConstructor_DoubleQuotedCells_Benchmark_1000by130Cells();
    void Case63_CsvGrid_SetColCount();
    void Case64_CsvGrid_AddRow();
    void Case65_CsvGrid_AddColumnToAllRows();
    void Case66_CsvGrid_InsertRow();
    void Case67_CsvGrid_InsertColumnInAllRows();
    void Case68_CsvGrid_ClearCells();
    void Case69_CsvGrid_ResetGrid();
    void Case70_CsvGrid_LoadFromCSVFile_1();
    void Case71_CsvGrid_LoadFromCSVFile_2();
    void Case72_CsvGrid_LoadFromCSVFile_3();
    void Case73_CsvGrid_SaveToCSVFile_1();
    void Case74_CsvGrid_SaveToCSVFile_2();
    void Case75_CsvGrid_OStream();
    void Case76_Cell_LongDoubleConstructor();
    void Case77_Cell_LongDoubleAssignment();
    void Case78_Cell_LongDoubleConvertException();
    void Case79_Cell_LongToDoubleDef_1();
    void Case80_Cell_LongToDoubleDef_2();
    void Case81_Row_InitializerListLongDoubleConstructor();
    void Case82_Row_AddColumnAsLongDouble();
    void Case83_Row_InsertColumnAsLongDouble();
};

CsvGridTest::CsvGridTest()
{
}

void CsvGridTest::initTestCase()
{
}

void CsvGridTest::cleanupTestCase()
{
}

void CsvGridTest::Case1_xCsvGridColOutOfRangeError_1()
{
    try
    {
        BOOST_THROW_EXCEPTION(core_lib::csv_grid::xCsvGridColOutOfRangeError());
    }
    catch(core_lib::csv_grid::xCsvGridColOutOfRangeError& e)
    {
        QCOMPARE(e.what(), "invalid column index");
        std::string info = boost::diagnostic_information(e);
        QVERIFY(info != "");
    }
}

void CsvGridTest::Case2_xCsvGridColOutOfRangeError_2()
{
    try
    {
        BOOST_THROW_EXCEPTION(core_lib::csv_grid::xCsvGridColOutOfRangeError("user defined message"));
    }
    catch(core_lib::csv_grid::xCsvGridColOutOfRangeError& e)
    {
        QCOMPARE(e.what(), "user defined message");
        std::string info = boost::diagnostic_information(e);
        QVERIFY(info != "");
    }
}

void CsvGridTest::Case3_xCsvGridDimensionError_1()
{
    try
    {
        BOOST_THROW_EXCEPTION(core_lib::csv_grid::xCsvGridDimensionError());
    }
    catch(core_lib::csv_grid::xCsvGridDimensionError& e)
    {
        QCOMPARE(e.what(), "rows and cols must be > 0");
        std::string info = boost::diagnostic_information(e);
        QVERIFY(info != "");
    }
}

void CsvGridTest::Case4_xCsvGridDimensionError_2()
{
    try
    {
        BOOST_THROW_EXCEPTION(core_lib::csv_grid::xCsvGridDimensionError("user defined message"));
    }
    catch(core_lib::csv_grid::xCsvGridDimensionError& e)
    {
        QCOMPARE(e.what(), "user defined message");
        std::string info = boost::diagnostic_information(e);
        QVERIFY(info != "");
    }
}

void CsvGridTest::Case5_xCsvGridRowOutOfRangeError_1()
{
    try
    {
        BOOST_THROW_EXCEPTION(core_lib::csv_grid::xCsvGridRowOutOfRangeError());
    }
    catch(core_lib::csv_grid::xCsvGridRowOutOfRangeError& e)
    {
        QCOMPARE(e.what(), "invalid row index");
        std::string info = boost::diagnostic_information(e);
        QVERIFY(info != "");
    }
}

void CsvGridTest::Case6_xCsvGridRowOutOfRangeError_2()
{
    try
    {
        BOOST_THROW_EXCEPTION(core_lib::csv_grid::xCsvGridRowOutOfRangeError("user defined message"));
    }
    catch(core_lib::csv_grid::xCsvGridRowOutOfRangeError& e)
    {
        QCOMPARE(e.what(), "user defined message");
        std::string info = boost::diagnostic_information(e);
        QVERIFY(info != "");
    }
}

void CsvGridTest::Case7_xCsvGridCreateFileStreamError_1()
{
    try
    {
        BOOST_THROW_EXCEPTION(core_lib::csv_grid::xCsvGridCreateFileStreamError());
    }
    catch(core_lib::csv_grid::xCsvGridCreateFileStreamError& e)
    {
        QCOMPARE(e.what(), "failed to create file stream");
        std::string info = boost::diagnostic_information(e);
        QVERIFY(info != "");
    }
}

void CsvGridTest::Case8_xCsvGridCreateFileStreamError_2()
{
    try
    {
        BOOST_THROW_EXCEPTION(core_lib::csv_grid::xCsvGridCreateFileStreamError("user defined message"));
    }
    catch(core_lib::csv_grid::xCsvGridCreateFileStreamError& e)
    {
        QCOMPARE(e.what(), "user defined message");
        std::string info = boost::diagnostic_information(e);
        QVERIFY(info != "");
    }
}

void CsvGridTest::Case9_Cell_DefaultConstructor()
{
    core_lib::csv_grid::Cell cell;
    std::string value = cell;
    QCOMPARE(value.c_str(), "");
}

void CsvGridTest::Case10_Cell_StringConstructor()
{
    core_lib::csv_grid::Cell cell("test string");
    std::string value = cell;
    QCOMPARE(value.c_str(), "test string");
}

void CsvGridTest::Case11_Cell_CopyConstructor()
{
    core_lib::csv_grid::Cell cell("test string");
    core_lib::csv_grid::Cell cellCopy(cell);
    std::string value = cell;
    std::string valueCopy = cellCopy;
    QCOMPARE(value.c_str(), valueCopy.c_str());
}

void CsvGridTest::Case12_Cell_MoveConstructor()
{
    core_lib::csv_grid::Cell cell(core_lib::csv_grid::Cell("test string"));
    std::string value = cell;
    QCOMPARE(value.c_str(), "test string");
}

void CsvGridTest::Case13_Cell_Int32Constructor()
{
    int32_t value1 = std::numeric_limits<int32_t>::max() / 2;
    core_lib::csv_grid::Cell cell(value1);
    int32_t value2 = cell;
    QCOMPARE(value1, value2);
}

void CsvGridTest::Case14_Cell_Int64Constructor()
{
    int64_t value1 = std::numeric_limits<int64_t>::max() / 2;
    core_lib::csv_grid::Cell cell(value1);
    int64_t value2 = cell;
    QCOMPARE(value1, value2);
}

void CsvGridTest::Case15_Cell_DoubleConstructor()
{
    double value1 = std::numeric_limits<double>::max() / 2;
    core_lib::csv_grid::Cell cell(value1);
    double value2 = cell;
    QCOMPARE(value1, value2);
}

void CsvGridTest::Case16_Cell_CopyAssignment()
{
    core_lib::csv_grid::Cell cell("test string");
    core_lib::csv_grid::Cell cellCopy;
    cellCopy = cell;
    std::string value = cell;
    std::string valueCopy = cellCopy;
    QCOMPARE(value.c_str(), valueCopy.c_str());
}

void CsvGridTest::Case17_Cell_MoveAssignment()
{
    core_lib::csv_grid::Cell cell;
    cell =  core_lib::csv_grid::Cell("test string");
    std::string value = cell;
    QCOMPARE(value.c_str(), "test string");
}

void CsvGridTest::Case18_Cell_StringAssignment()
{
    core_lib::csv_grid::Cell cell;
    std::string value = "test string";
    cell = value;
    std::string cellValue = cell;
    QCOMPARE(cellValue.c_str(), value.c_str());
}

void CsvGridTest::Case19_Cell_Int32Assignment()
{
    core_lib::csv_grid::Cell cell;
    int32_t value1 = std::numeric_limits<int32_t>::max() / 2;
    cell = value1;
    int32_t value2 = cell;
    QCOMPARE(value1, value2);
}

void CsvGridTest::Case20_Cell_Int64Assignment()
{
    core_lib::csv_grid::Cell cell;
    int64_t value1 = std::numeric_limits<int64_t>::max() / 2;
    cell = value1;
    int64_t value2 = cell;
    QCOMPARE(value1, value2);
}

void CsvGridTest::Case21_Cell_DoubleAssignment()
{
    core_lib::csv_grid::Cell cell;
    double value1 = std::numeric_limits<double>::max() / 2;
    cell = value1;
    double value2 = cell;
    QCOMPARE(value1, value2);
}

void CsvGridTest::Case22_Cell_Int32ConvertException()
{
    bool correctException;

    try
    {
        core_lib::csv_grid::Cell cell("I'm not a number I'm a free man!");
        int32_t temp = static_cast<int32_t>(cell);
        (void)temp;
        correctException = false;
    }
    catch(std::invalid_argument& e)
    {
        (void)e;
        correctException = true;
    }
    catch(...)
    {
        correctException = false;
    }

    QVERIFY(correctException);
}

void CsvGridTest::Case23_Cell_Int64ConvertException()
{
    bool correctException;

    try
    {
        core_lib::csv_grid::Cell cell("I'm not a number I'm a free man!");
        int64_t temp = static_cast<int64_t>(cell);
        (void)temp;
        correctException = false;
    }
    catch(std::invalid_argument& e)
    {
        (void)e;
        correctException = true;
    }
    catch(...)
    {
        correctException = false;
    }

    QVERIFY(correctException);
}

void CsvGridTest::Case24_Cell_DoubleConvertException()
{
    bool correctException;

    try
    {
        core_lib::csv_grid::Cell cell("I'm not a number I'm a free man!");
        double temp = static_cast<double>(cell);
        (void)temp;
        correctException = false;
    }
    catch(std::invalid_argument& e)
    {
        (void)e;
        correctException = true;
    }
    catch(...)
    {
        correctException = false;
    }

    QVERIFY(correctException);
}

void CsvGridTest::Case25_Cell_ToInt32Def_1()
{
    core_lib::csv_grid::Cell cell("I'm not a number I'm a free man!");
    const int32_t testValue = std::numeric_limits<int32_t>::max() / 2;
    int32_t value = cell.ToInt32Def(testValue);
    QCOMPARE(value, testValue);
}

void CsvGridTest::Case26_Cell_ToInt32Def_2()
{
    const int32_t testValue = std::numeric_limits<int32_t>::max() / 2;
    core_lib::csv_grid::Cell cell(testValue);
    int32_t value = cell.ToInt32Def(0);
    QCOMPARE(value, testValue);
}

void CsvGridTest::Case27_Cell_ToInt64Def_1()
{
    core_lib::csv_grid::Cell cell("I'm not a number I'm a free man!");
    const int64_t testValue = std::numeric_limits<int64_t>::max() / 2;
    int64_t value = cell.ToInt64Def(testValue);
    QCOMPARE(value, testValue);
}

void CsvGridTest::Case28_Cell_ToInt64Def_2()
{
    const int64_t testValue = std::numeric_limits<int64_t>::max() / 2;
    core_lib::csv_grid::Cell cell(testValue);
    int64_t value = cell.ToInt64Def(0);
    QCOMPARE(value, testValue);
}

void CsvGridTest::Case29_Cell_ToDoubleDef_1()
{
    core_lib::csv_grid::Cell cell("I'm not a number I'm a free man!");
    const double testValue = std::numeric_limits<double>::max() / 2;
    double value = cell.ToDoubleDef(testValue);
    QCOMPARE(value, testValue);
}

void CsvGridTest::Case30_Cell_ToDoubleDef_2()
{
    const double testValue = std::numeric_limits<double>::max() / 2;
    core_lib::csv_grid::Cell cell(testValue);
    double value = cell.ToDoubleDef(0);
    QCOMPARE(value, testValue);
}

void CsvGridTest::Case31_Row_DefaultConstructor()
{
    core_lib::csv_grid::Row row;
    QCOMPARE(row.GetSize(), static_cast<size_t>(0));
}

void CsvGridTest::Case32_Row_CopyConstructor()
{
    core_lib::csv_grid::Row row;
    row.SetSize(100);
    QCOMPARE(row.GetSize(), static_cast<size_t>(100));
    core_lib::csv_grid::Row rowCopy(row);
    QCOMPARE(row.GetSize(), rowCopy.GetSize());
}

void CsvGridTest::Case33_Row_MoveConstructor()
{
    core_lib::csv_grid::Row row(core_lib::csv_grid::Row(100));
    QCOMPARE(row.GetSize(), static_cast<size_t>(100));
}

void CsvGridTest::Case34_Row_InitializingConstructor()
{
    core_lib::csv_grid::Row row(666);
    QCOMPARE(row.GetSize(), static_cast<size_t>(666));
}

void CsvGridTest::Case35_Row_InitializerListCellConstructor()
{
    core_lib::csv_grid::Row row = { Cell(), Cell(), Cell(), Cell(), Cell() };
    QCOMPARE(row.GetSize(), static_cast<size_t>(5));
}

void CsvGridTest::Case36_Row_InitializerListStringConstructor()
{
    core_lib::csv_grid::Row row = { Cell(""), Cell(""), Cell(""), Cell(""), Cell("") };
    QCOMPARE(row.GetSize(), static_cast<size_t>(5));
}

void CsvGridTest::Case37_Row_InitializerListInt32Constructor()
{
    int32_t i = 0;
    core_lib::csv_grid::Row row = { Cell(i), Cell(i), Cell(i), Cell(i), Cell(i) };
    QCOMPARE(row.GetSize(), static_cast<size_t>(5));
}

void CsvGridTest::Case38_Row_InitializerListInt64Constructor()
{
    int64_t i = 0;
    core_lib::csv_grid::Row row = { Cell(i), Cell(i), Cell(i), Cell(i), Cell(i) };
    QCOMPARE(row.GetSize(), static_cast<size_t>(5));
}

void CsvGridTest::Case39_Row_InitializerListDoubleConstructor()
{
    double i = 0;
    core_lib::csv_grid::Row row = { Cell(i), Cell(i), Cell(i), Cell(i), Cell(i) };
    QCOMPARE(row.GetSize(), static_cast<size_t>(5));
}

void CsvGridTest::Case40_Row_CopyAssignment()
{
    core_lib::csv_grid::Row row = { Cell(1), Cell(2), Cell(3), Cell(4), Cell(5) };
    core_lib::csv_grid::Row rowCopy;
    rowCopy = row;
    QCOMPARE(row.GetSize(), static_cast<size_t>(5));
}

void CsvGridTest::Case41_Row_MoveAssignment()
{
    core_lib::csv_grid::Row row;
    row = core_lib::csv_grid::Row(100);
    QCOMPARE(row.GetSize(), static_cast<size_t>(100));
}

void CsvGridTest::Case42_Row_SubscriptOperator()
{
    core_lib::csv_grid::Row row = { Cell(1), Cell(2), Cell(3), Cell(4), Cell(5) };
    QCOMPARE(static_cast<int32_t>(row[0]), 1);
    QCOMPARE(static_cast<int32_t>(row[1]), 2);
    QCOMPARE(static_cast<int32_t>(row[2]), 3);
    QCOMPARE(static_cast<int32_t>(row[3]), 4);
    QCOMPARE(static_cast<int32_t>(row[4]), 5);

    bool exceptionThrown;

    try
    {
        int32_t temp = static_cast<int32_t>(row[6]);
        (void)temp;
        exceptionThrown = false;
    }
    catch(...)
    {
        exceptionThrown = true;
    }

    QVERIFY(exceptionThrown);
}

void CsvGridTest::Case43_Row_AddColumnAsString()
{
    core_lib::csv_grid::Row row;
    QCOMPARE(row.GetSize(), static_cast<size_t>(0));
    row.AddColumn("new entry");
    QCOMPARE(row.GetSize(), static_cast<size_t>(1));
    QCOMPARE(static_cast<std::string>(row[0]), std::string("new entry"));
}

void CsvGridTest::Case44_Row_AddColumnAsInt32()
{
    core_lib::csv_grid::Row row;
    QCOMPARE(row.GetSize(), static_cast<size_t>(0));
    row.AddColumn(int32_t(100));
    QCOMPARE(row.GetSize(), static_cast<size_t>(1));
    QCOMPARE(static_cast<int32_t>(row[0]), int32_t(100));
}

void CsvGridTest::Case45_Row_AddColumnAsInt64()
{
    core_lib::csv_grid::Row row;
    QCOMPARE(row.GetSize(), static_cast<size_t>(0));
    row.AddColumn(int64_t(100));
    QCOMPARE(row.GetSize(), static_cast<size_t>(1));
    QCOMPARE(static_cast<int64_t>(row[0]), int64_t(100));
}

void CsvGridTest::Case46_Row_AddColumnAsDouble()
{
    core_lib::csv_grid::Row row;
    QCOMPARE(row.GetSize(), static_cast<size_t>(0));
    row.AddColumn(double(100.1));
    QCOMPARE(row.GetSize(), static_cast<size_t>(1));
    QCOMPARE(static_cast<double>(row[0]), double(100.1));
}

void CsvGridTest::Case47_Row_InsertColumnAsString()
{
    core_lib::csv_grid::Row row = { Cell("1"), Cell("2"), Cell("3"), Cell("4"), Cell("5") };
    row.InsertColumn(3, "666");
    QCOMPARE(row.GetSize(), static_cast<size_t>(6));
    QCOMPARE(static_cast<std::string>(row[0]), std::string("1"));
    QCOMPARE(static_cast<std::string>(row[1]), std::string("2"));
    QCOMPARE(static_cast<std::string>(row[2]), std::string("3"));
    QCOMPARE(static_cast<std::string>(row[3]), std::string("666"));
    QCOMPARE(static_cast<std::string>(row[4]), std::string("4"));
    QCOMPARE(static_cast<std::string>(row[5]), std::string("5"));

    bool exceptionThrown;

    try
    {
        row.InsertColumn(100, "667");
        exceptionThrown = false;
    }
    catch(core_lib::csv_grid::xCsvGridColOutOfRangeError& e)
    {
        (void)e;
        exceptionThrown = true;
    }
    catch(...)
    {
        exceptionThrown = false;
    }

    QVERIFY(exceptionThrown);
}

void CsvGridTest::Case48_Row_InsertColumnAsInt32()
{
    core_lib::csv_grid::Row row = { Cell(1), Cell(2), Cell(3), Cell(4), Cell(5) };
    row.InsertColumn(3, static_cast<int32_t>(666));
    QCOMPARE(row.GetSize(), static_cast<size_t>(6));
    QCOMPARE(static_cast<int32_t>(row[0]), int32_t(1));
    QCOMPARE(static_cast<int32_t>(row[1]), int32_t(2));
    QCOMPARE(static_cast<int32_t>(row[2]), int32_t(3));
    QCOMPARE(static_cast<int32_t>(row[3]), int32_t(666));
    QCOMPARE(static_cast<int32_t>(row[4]), int32_t(4));
    QCOMPARE(static_cast<int32_t>(row[5]), int32_t(5));
}

void CsvGridTest::Case49_Row_InsertColumnAsInt64()
{
    core_lib::csv_grid::Row row = { Cell(1), Cell(2), Cell(3), Cell(4), Cell(5) };
    row.InsertColumn(3, static_cast<int64_t>(666));
    QCOMPARE(row.GetSize(), static_cast<size_t>(6));
    QCOMPARE(static_cast<int64_t>(row[0]), int64_t(1));
    QCOMPARE(static_cast<int64_t>(row[1]), int64_t(2));
    QCOMPARE(static_cast<int64_t>(row[2]), int64_t(3));
    QCOMPARE(static_cast<int64_t>(row[3]), int64_t(666));
    QCOMPARE(static_cast<int64_t>(row[4]), int64_t(4));
    QCOMPARE(static_cast<int64_t>(row[5]), int64_t(5));
}

void CsvGridTest::Case50_Row_InsertColumnAsDouble()
{
    core_lib::csv_grid::Row row = { Cell(1.1), Cell(2.2), Cell(3.3), Cell(4.4), Cell(5.5) };
    row.InsertColumn(3, static_cast<double>(666.6));
    QCOMPARE(row.GetSize(), static_cast<size_t>(6));
    QCOMPARE(static_cast<double>(row[0]), double(1.1));
    QCOMPARE(static_cast<double>(row[1]), double(2.2));
    QCOMPARE(static_cast<double>(row[2]), double(3.3));
    QCOMPARE(static_cast<double>(row[3]), double(666.6));
    QCOMPARE(static_cast<double>(row[4]), double(4.4));
    QCOMPARE(static_cast<double>(row[5]), double(5.5));
}

void CsvGridTest::Case51_Row_ClearCells()
{
    core_lib::csv_grid::Row row = { Cell(1), Cell(2), Cell(3), Cell(4), Cell(5) };
    QCOMPARE(row.GetSize(), static_cast<size_t>(5));
    row.ClearCells();
    QCOMPARE(row.GetSize(), static_cast<size_t>(5));
    QCOMPARE(static_cast<std::string>(row[0]), std::string(""));
    QCOMPARE(static_cast<std::string>(row[1]), std::string(""));
    QCOMPARE(static_cast<std::string>(row[2]), std::string(""));
    QCOMPARE(static_cast<std::string>(row[3]), std::string(""));
    QCOMPARE(static_cast<std::string>(row[4]), std::string(""));

}

void CsvGridTest::Case52_Row_ResetRow()
{
    core_lib::csv_grid::Row row = { Cell(1), Cell(2), Cell(3), Cell(4), Cell(5) };
    QCOMPARE(row.GetSize(), static_cast<size_t>(5));
    row.ResetRow();
    QCOMPARE(row.GetSize(), static_cast<size_t>(0));
}

void CsvGridTest::Case53_CsvGrid_DefaultConstructor()
{
    core_lib::csv_grid::CsvGrid grid;
    QCOMPARE(grid.GetRowCount(), static_cast<size_t>(0));
}

void CsvGridTest::Case54_CsvGrid_InitializingConstructor_1()
{
    core_lib::csv_grid::CsvGrid grid(10, 10);
    QCOMPARE(grid.GetRowCount(), static_cast<size_t>(10));

    for(size_t row = 0; row < grid.GetRowCount(); ++row)
    {
        QCOMPARE(grid.GetColCount(row), static_cast<size_t>(10));
        QCOMPARE(grid[row].GetSize(), static_cast<size_t>(10));
    }
}

void CsvGridTest::Case55_CsvGrid_InitializingConstructor_2()
{
    bool exceptionThrown;

    try
    {
        core_lib::csv_grid::CsvGrid grid(0, 0);
        exceptionThrown = false;
    }
    catch(core_lib::csv_grid::xCsvGridDimensionError& e)
    {
        (void)e;
        exceptionThrown = true;
    }
    catch(...)
    {
        exceptionThrown = false;
    }

    QVERIFY(exceptionThrown);
}

void CsvGridTest::Case56_CsvGrid_InitializerListConstructor()
{
    core_lib::csv_grid::Row row = { Cell(1), Cell(2), Cell(3), Cell(4), Cell(5) };
    core_lib::csv_grid::CsvGrid grid = { row, row, row, row, row };

    QCOMPARE(grid.GetRowCount(), static_cast<size_t>(5));

    for(size_t r = 0; r < grid.GetRowCount(); ++r)
    {
        QCOMPARE(grid.GetColCount(r), static_cast<size_t>(5));
        QCOMPARE(grid[r].GetSize(), static_cast<size_t>(5));
    }
}

void CsvGridTest::Case57_CsvGrid_CopyConstructor()
{
    core_lib::csv_grid::Row row = { Cell(1), Cell(2), Cell(3), Cell(4), Cell(5) };
    core_lib::csv_grid::CsvGrid grid = { row, row, row, row, row };
    core_lib::csv_grid::CsvGrid gridCopy(grid);

    QCOMPARE(gridCopy.GetRowCount(), static_cast<size_t>(5));

    for(size_t r = 0; r < gridCopy.GetRowCount(); ++r)
    {
        QCOMPARE(gridCopy.GetColCount(r), static_cast<size_t>(5));
        QCOMPARE(gridCopy[r].GetSize(), static_cast<size_t>(5));
    }
}

void CsvGridTest::Case58_CsvGrid_MoveConstructor()
{
    core_lib::csv_grid::Row row = { Cell(1), Cell(2), Cell(3), Cell(4), Cell(5) };
    core_lib::csv_grid::CsvGrid gridCopy(core_lib::csv_grid::CsvGrid{row, row, row, row, row });

    QCOMPARE(gridCopy.GetRowCount(), static_cast<size_t>(5));

    for(size_t r = 0; r < gridCopy.GetRowCount(); ++r)
    {
        QCOMPARE(gridCopy.GetColCount(r), static_cast<size_t>(5));
        QCOMPARE(gridCopy[r].GetSize(), static_cast<size_t>(5));
    }
}

void CsvGridTest::Case59_CsvGrid_FileConstructor_SimpleCells()
{
    try
    {
#if BOOST_OS_WINDOWS
        core_lib::csv_grid::CsvGrid grid("../testfile1.csv", core_lib::csv_grid::eCellFormatOptions::simpleCells);
#else
        core_lib::csv_grid::CsvGrid grid("../../testfile1.csv", core_lib::csv_grid::eCellFormatOptions::simpleCells);
#endif
        QCOMPARE(grid.GetRowCount(), static_cast<size_t>(1000));
        QCOMPARE(grid[0].GetSize(), static_cast<size_t>(130));
        QCOMPARE(grid[999].GetSize(), static_cast<size_t>(130));
        QCOMPARE(static_cast<std::string>(grid[0][0]), std::string("I am a test string"));
        QCOMPARE(static_cast<std::string>(grid[999][129]), std::string("I am a test string"));
    }
    catch(...)
    {
        QFAIL("unexpected exception caught when loading csv file");
    }
}

void CsvGridTest::Case60_CsvGrid_FileConstructor_SimpleCells_Benchmark_1000by130Cells()
{
    try
    {
        QBENCHMARK
        {
#if BOOST_OS_WINDOWS
            core_lib::csv_grid::CsvGrid grid("../testfile1.csv", core_lib::csv_grid::eCellFormatOptions::simpleCells);
#else
            core_lib::csv_grid::CsvGrid grid("../../testfile1.csv", core_lib::csv_grid::eCellFormatOptions::simpleCells);
#endif
        }
    }
    catch(...)
    {
        QFAIL("unexpected exception caught when loading csv file");
    }
}

void CsvGridTest::Case61_CsvGrid_FileConstructor_DoubleQuotedCells()
{
    try
    {
#if BOOST_OS_WINDOWS
        core_lib::csv_grid::CsvGrid grid("../testfile2.csv", core_lib::csv_grid::eCellFormatOptions::doubleQuotedCells);
#else
        core_lib::csv_grid::CsvGrid grid("../../testfile2.csv", core_lib::csv_grid::eCellFormatOptions::doubleQuotedCells);
#endif
        QCOMPARE(grid.GetRowCount(), static_cast<size_t>(1000));
        QCOMPARE(grid[0].GetSize(), static_cast<size_t>(130));
        QCOMPARE(grid[999].GetSize(), static_cast<size_t>(130));
        QCOMPARE(static_cast<std::string>(grid[0][0]), std::string("I am a test, string"));
        QCOMPARE(static_cast<std::string>(grid[999][129]), std::string("I am a test, string"));
    }
    catch(...)
    {
        QFAIL("unexpected exception caught when loading csv file");
    }
}

void CsvGridTest::Case62_CsvGrid_FileConstructor_DoubleQuotedCells_Benchmark_1000by130Cells()
{
    try
    {
        QBENCHMARK
        {
#if BOOST_OS_WINDOWS
            core_lib::csv_grid::CsvGrid grid("../testfile2.csv", core_lib::csv_grid::eCellFormatOptions::doubleQuotedCells);
#else
            core_lib::csv_grid::CsvGrid grid("../../testfile2.csv", core_lib::csv_grid::eCellFormatOptions::doubleQuotedCells);
#endif
        }
    }
    catch(...)
    {
        QFAIL("unexpected exception caught when loading csv file");
    }
}

void CsvGridTest::Case63_CsvGrid_SetColCount()
{
    core_lib::csv_grid::CsvGrid grid(10, 10);
    QCOMPARE(grid.GetRowCount(), static_cast<size_t>(10));

    for(size_t row = 0; row < grid.GetRowCount(); ++row)
    {
        QCOMPARE(grid.GetColCount(row), static_cast<size_t>(10));
        QCOMPARE(grid[row].GetSize(), static_cast<size_t>(10));
    }

    grid.SetRowCount(15, 5);

    QCOMPARE(grid.GetRowCount(), static_cast<size_t>(15));

    for(size_t row = 0; row < grid.GetRowCount(); ++row)
    {
        if (row < 10)
        {
            QCOMPARE(grid.GetColCount(row), static_cast<size_t>(10));
            QCOMPARE(grid[row].GetSize(), static_cast<size_t>(10));
        }
        else
        {
            QCOMPARE(grid.GetColCount(row), static_cast<size_t>(5));
            QCOMPARE(grid[row].GetSize(), static_cast<size_t>(5));
        }
    }

    grid.SetRowCount(5, 5);

    QCOMPARE(grid.GetRowCount(), static_cast<size_t>(5));

    for(size_t row = 0; row < grid.GetRowCount(); ++row)
    {
        QCOMPARE(grid.GetColCount(row), static_cast<size_t>(10));
        QCOMPARE(grid[row].GetSize(), static_cast<size_t>(10));
    }
}

void CsvGridTest::Case64_CsvGrid_AddRow()
{
    core_lib::csv_grid::CsvGrid grid(10, 10);
    grid.AddRow(5);

    QCOMPARE(grid.GetRowCount(), static_cast<size_t>(11));

    for(size_t row = 0; row < grid.GetRowCount(); ++row)
    {
        if (row < 10)
        {
            QCOMPARE(grid.GetColCount(row), static_cast<size_t>(10));
            QCOMPARE(grid[row].GetSize(), static_cast<size_t>(10));
        }
        else
        {
            QCOMPARE(grid.GetColCount(row), static_cast<size_t>(5));
            QCOMPARE(grid[row].GetSize(), static_cast<size_t>(5));
        }
    }
}

void CsvGridTest::Case65_CsvGrid_AddColumnToAllRows()
{
    core_lib::csv_grid::Row row1 = { Cell(1) };
    core_lib::csv_grid::Row row2 = { Cell(1), Cell(2) };
    core_lib::csv_grid::Row row3 = { Cell(1), Cell(2), Cell(3) };
    core_lib::csv_grid::CsvGrid grid = {row1, row2, row3};

    grid.AddColumnToAllRows();

    QCOMPARE(grid[0].GetSize(), static_cast<size_t>(2));
    QCOMPARE(grid[1].GetSize(), static_cast<size_t>(3));
    QCOMPARE(grid[2].GetSize(), static_cast<size_t>(4));
}

void CsvGridTest::Case66_CsvGrid_InsertRow()
{
    core_lib::csv_grid::Row row1 = { Cell(1) };
    core_lib::csv_grid::Row row2 = { Cell(1), Cell(2) };
    core_lib::csv_grid::Row row3 = { Cell(1), Cell(2), Cell(3) };
    core_lib::csv_grid::CsvGrid grid = {row1, row2, row3};

    grid.InsertRow(1, 5);
    grid.InsertRow(2);

    QCOMPARE(grid.GetRowCount(), static_cast<size_t>(5));

    QCOMPARE(grid.GetColCount(0), static_cast<size_t>(1));
    QCOMPARE(grid[0].GetSize(), static_cast<size_t>(1));

    QCOMPARE(grid.GetColCount(1), static_cast<size_t>(5));
    QCOMPARE(grid[1].GetSize(), static_cast<size_t>(5));

    QCOMPARE(grid.GetColCount(2), static_cast<size_t>(0));
    QCOMPARE(grid[2].GetSize(), static_cast<size_t>(0));

    QCOMPARE(grid.GetColCount(3), static_cast<size_t>(2));
    QCOMPARE(grid[3].GetSize(), static_cast<size_t>(2));

    QCOMPARE(grid.GetColCount(4), static_cast<size_t>(3));
    QCOMPARE(grid[4].GetSize(), static_cast<size_t>(3));

    bool correctException;

    try
    {
        grid.InsertRow(100);
        correctException = false;
    }
    catch(core_lib::csv_grid::xCsvGridRowOutOfRangeError& e)
    {
        (void)e;
        correctException = true;
    }
    catch(...)
    {
        correctException = false;
    }

    QVERIFY(correctException);
}

void CsvGridTest::Case67_CsvGrid_InsertColumnInAllRows()
{
    core_lib::csv_grid::Row row1 = { Cell(1), Cell(2) };
    core_lib::csv_grid::Row row2 = { Cell(1), Cell(2), Cell(3) };
    core_lib::csv_grid::Row row3 = { Cell(1), Cell(2), Cell(3), Cell(4) };
    core_lib::csv_grid::CsvGrid grid = {row1, row2, row3};

    grid.InsertColumnInAllRows(1);

    QCOMPARE(grid.GetColCount(0), static_cast<size_t>(3));
    QCOMPARE(grid[0].GetSize(), static_cast<size_t>(3));

    QCOMPARE(grid.GetColCount(1), static_cast<size_t>(4));
    QCOMPARE(grid[1].GetSize(), static_cast<size_t>(4));

    QCOMPARE(grid.GetColCount(2), static_cast<size_t>(5));
    QCOMPARE(grid[2].GetSize(), static_cast<size_t>(5));

    std::string cell = static_cast<std::string>(grid[0][1]);
    QCOMPARE(cell.c_str(), "");

    cell = static_cast<std::string>(grid[1][1]);
    QCOMPARE(cell.c_str(), "");

    cell = static_cast<std::string>(grid[2][1]);
    QCOMPARE(cell.c_str(), "");

    try
    {
        grid.InsertColumnInAllRows(100);

        QCOMPARE(grid.GetColCount(0), static_cast<size_t>(3));
        QCOMPARE(grid[0].GetSize(), static_cast<size_t>(3));

        QCOMPARE(grid.GetColCount(1), static_cast<size_t>(4));
        QCOMPARE(grid[1].GetSize(), static_cast<size_t>(4));

        QCOMPARE(grid.GetColCount(2), static_cast<size_t>(5));
        QCOMPARE(grid[2].GetSize(), static_cast<size_t>(5));
    }
    catch(...)
    {
        QFAIL("unexpected exception caught when inserting column to all rows");
    }
}

void CsvGridTest::Case68_CsvGrid_ClearCells()
{
    core_lib::csv_grid::Row row1 = { Cell(1), Cell(2) };
    core_lib::csv_grid::Row row2 = { Cell(1), Cell(2), Cell(3) };
    core_lib::csv_grid::Row row3 = { Cell(1), Cell(2), Cell(3), Cell(4) };
    core_lib::csv_grid::CsvGrid grid = {row1, row2, row3};

    grid.ClearCells();

    QCOMPARE(grid.GetRowCount(), static_cast<size_t>(3));

    QCOMPARE(grid.GetColCount(0), static_cast<size_t>(2));
    QCOMPARE(grid[0].GetSize(), static_cast<size_t>(2));

    QCOMPARE(grid.GetColCount(1), static_cast<size_t>(3));
    QCOMPARE(grid[1].GetSize(), static_cast<size_t>(3));

    QCOMPARE(grid.GetColCount(2), static_cast<size_t>(4));
    QCOMPARE(grid[2].GetSize(), static_cast<size_t>(4));

    for (size_t row = 0; row < grid.GetRowCount(); ++row)
    {
        for (size_t col = 0; col < grid.GetColCount(row); ++col)
        {
            std::string value = grid[row][col];
            QCOMPARE(value.c_str(), "");
        }
    }
}

void CsvGridTest::Case69_CsvGrid_ResetGrid()
{
    core_lib::csv_grid::Row row1 = { Cell(1), Cell(2) };
    core_lib::csv_grid::Row row2 = { Cell(1), Cell(2), Cell(3) };
    core_lib::csv_grid::Row row3 = { Cell(1), Cell(2), Cell(3), Cell(4) };
    core_lib::csv_grid::CsvGrid grid = {row1, row2, row3};

    grid.ResetGrid();

    QCOMPARE(grid.GetRowCount(), static_cast<size_t>(0));
}

void CsvGridTest::Case70_CsvGrid_LoadFromCSVFile_1()
{
    core_lib::csv_grid::CsvGrid grid;
    bool correctException;

    try
    {
        grid.LoadFromCSVFile("dummyfile.csv", core_lib::csv_grid::eCellFormatOptions::simpleCells);
        correctException = false;
    }
    catch(core_lib::csv_grid::xCsvGridCreateFileStreamError& e)
    {
        (void)e;
        correctException = true;
    }
    catch(...)
    {
        correctException = false;
    }

    QVERIFY(correctException);
}

void CsvGridTest::Case71_CsvGrid_LoadFromCSVFile_2()
{
    core_lib::csv_grid::CsvGrid grid;
    try
    {
#if BOOST_OS_WINDOWS
        grid.LoadFromCSVFile("../testfile1.csv", core_lib::csv_grid::eCellFormatOptions::simpleCells);
#else
        grid.LoadFromCSVFile("../../testfile1.csv", core_lib::csv_grid::eCellFormatOptions::simpleCells);
#endif

        QCOMPARE(grid.GetRowCount(), static_cast<size_t>(1000));
        QCOMPARE(grid[0].GetSize(), static_cast<size_t>(130));
        QCOMPARE(grid[999].GetSize(), static_cast<size_t>(130));
        QCOMPARE(static_cast<std::string>(grid[0][0]), std::string("I am a test string"));
        QCOMPARE(static_cast<std::string>(grid[999][129]), std::string("I am a test string"));
    }
    catch(...)
    {
        QFAIL("unexpected exception caught when loading from file");
    }
}

void CsvGridTest::Case72_CsvGrid_LoadFromCSVFile_3()
{
    core_lib::csv_grid::Row row1 = { Cell(1), Cell(2) };
    core_lib::csv_grid::Row row2 = { Cell(1), Cell(2), Cell(3) };
    core_lib::csv_grid::Row row3 = { Cell(1), Cell(2), Cell(3), Cell(4) };
    core_lib::csv_grid::CsvGrid grid = {row1, row2, row3};

    QCOMPARE(grid.GetRowCount(), static_cast<size_t>(3));

    QCOMPARE(grid.GetColCount(0), static_cast<size_t>(2));
    QCOMPARE(grid[0].GetSize(), static_cast<size_t>(2));

    QCOMPARE(grid.GetColCount(1), static_cast<size_t>(3));
    QCOMPARE(grid[1].GetSize(), static_cast<size_t>(3));

    QCOMPARE(grid.GetColCount(2), static_cast<size_t>(4));
    QCOMPARE(grid[2].GetSize(), static_cast<size_t>(4));


    try
    {
#if BOOST_OS_WINDOWS
        grid.LoadFromCSVFile("../testfile1.csv", core_lib::csv_grid::eCellFormatOptions::simpleCells);
#else
        grid.LoadFromCSVFile("../../testfile1.csv", core_lib::csv_grid::eCellFormatOptions::simpleCells);
#endif

        QCOMPARE(grid.GetRowCount(), static_cast<size_t>(1000));
        QCOMPARE(grid[0].GetSize(), static_cast<size_t>(130));
        QCOMPARE(grid[999].GetSize(), static_cast<size_t>(130));
        QCOMPARE(static_cast<std::string>(grid[0][0]), std::string("I am a test string"));
        QCOMPARE(static_cast<std::string>(grid[999][129]), std::string("I am a test string"));
    }
    catch(...)
    {
        QFAIL("unexpected exception caught when loading from file");
    }
}

void CsvGridTest::Case73_CsvGrid_SaveToCSVFile_1()
{
    core_lib::csv_grid::Row row1 = { Cell(1), Cell(2) };
    core_lib::csv_grid::Row row2 = { Cell(1), Cell(2), Cell(3) };
    core_lib::csv_grid::Row row3 = { Cell(1), Cell(2), Cell(3), Cell(4) };
    core_lib::csv_grid::CsvGrid gridOut = {row1, row2, row3};

    try
    {
        gridOut.SaveToCsvFile("testSave.csv", core_lib::csv_grid::eSaveToFileOptions::truncate);
    }
    catch(...)
    {
        QFAIL("unexpected exception caught when saving to file");
    }

    try
    {
        core_lib::csv_grid::CsvGrid gridIn("testSave.csv", core_lib::csv_grid::eCellFormatOptions::simpleCells);

        QCOMPARE(gridIn.GetRowCount(), gridOut.GetRowCount());

        for (size_t row = 0; row < gridIn.GetRowCount(); ++row)
        {
            QCOMPARE(gridIn.GetColCount(row), gridOut.GetColCount(row));

            for (size_t col = 0; col < gridIn.GetColCount(row); ++col)
            {
                std::string valueIn = gridIn[row][col];
                std::string valueOut = gridOut[row][col];
                QCOMPARE(valueIn, valueOut);
            }
        }

        ::remove("testSave.csv");
    }
    catch(...)
    {
        QFAIL("unexpected exception caught when loading from file");
    }
}

void CsvGridTest::Case74_CsvGrid_SaveToCSVFile_2()
{
    core_lib::csv_grid::Row row1 = { Cell(1), Cell(2) };
    core_lib::csv_grid::Row row2 = { Cell("1,/nbum"), Cell("2"), Cell("3") };
    core_lib::csv_grid::Row row3 = { Cell(1), Cell(2), Cell(3), Cell(4) };
    core_lib::csv_grid::CsvGrid gridOut = {row1, row2, row3};

    try
    {
        gridOut.SaveToCsvFile("testSave.csv", core_lib::csv_grid::eSaveToFileOptions::truncate);
    }
    catch(...)
    {
        QFAIL("unexpected exception caught when saving to file");
    }

    try
    {
        core_lib::csv_grid::CsvGrid gridIn("testSave.csv", core_lib::csv_grid::eCellFormatOptions::doubleQuotedCells);

        QCOMPARE(gridIn.GetRowCount(), gridOut.GetRowCount());

        for (size_t row = 0; row < gridIn.GetRowCount(); ++row)
        {
            QCOMPARE(gridIn.GetColCount(row), gridOut.GetColCount(row));

            for (size_t col = 0; col < gridIn.GetColCount(row); ++col)
            {
                std::string valueIn = gridIn[row][col];
                std::string valueOut = gridOut[row][col];
                QCOMPARE(valueIn, valueOut);
            }
        }

        ::remove("testSave.csv");
    }
    catch(...)
    {
        QFAIL("unexpected exception caught when loading from file");
    }
}

void CsvGridTest::Case75_CsvGrid_OStream()
{
    // Deprecated

    /*core_lib::csv_grid::Row row1 = { 1, 2 };
    core_lib::csv_grid::Row row2 = { "1,/nbum", "2", "3" };
    core_lib::csv_grid::Row row3 = { 1, 2, 3, 4 };
    core_lib::csv_grid::CsvGrid gridOut = {row1, row2, row3};

    try
    {
        std::ofstream csvfile("testSave.csv", std::ios::trunc);

        if (!csvfile.is_open())
            BOOST_THROW_EXCEPTION(
                        core_lib::csv_grid::xCsvGridCreateFileStreamError(std::string("failed to create file stream for saving: ")
                                                      + "testSave.csv"));

        csvfile << gridOut;
    }
    catch(...)
    {
        QFAIL("unexpected exception caught when saving to file");
    }

    try
    {
        core_lib::csv_grid::CsvGrid gridIn("testSave.csv", core_lib::csv_grid::eCellFormatOptions::doubleQuotedCells);

        QCOMPARE(gridIn.GetRowCount(), gridOut.GetRowCount());

        for (size_t row = 0; row < gridIn.GetRowCount(); ++row)
        {
            QCOMPARE(gridIn.GetColCount(row), gridOut.GetColCount(row));

            for (size_t col = 0; col < gridIn.GetColCount(row); ++col)
            {
                std::string valueIn = gridIn[row][col];
                std::string valueOut = gridOut[row][col];
                QCOMPARE(valueIn, valueOut);
            }
        }

        ::remove("testSave.csv");
    }
    catch(...)
    {
        QFAIL("unexpected exception caught when loading from file");
    }*/
}

void CsvGridTest::Case76_Cell_LongDoubleConstructor()
{
    long double value1 = 1000000000.0L;
    core_lib::csv_grid::Cell cell(value1);
    long double value2 = cell;
    QCOMPARE(value1, value2);
}

void CsvGridTest::Case77_Cell_LongDoubleAssignment()
{
    core_lib::csv_grid::Cell cell;
    long double value1 = 1000000000.0L;
    cell = value1;
    long double value2 = cell;
    QCOMPARE(value1, value2);
}

void CsvGridTest::Case78_Cell_LongDoubleConvertException()
{
    bool correctException;

    try
    {
        core_lib::csv_grid::Cell cell("I'm not a number I'm a free man!");
        long double temp = static_cast<long double>(cell);
        (void)temp;
        correctException = false;
    }
    catch(std::invalid_argument& e)
    {
        (void)e;
        correctException = true;
    }
    catch(...)
    {
        correctException = false;
    }

    QVERIFY(correctException);
}

void CsvGridTest::Case79_Cell_LongToDoubleDef_1()
{
    core_lib::csv_grid::Cell cell("I'm not a number I'm a free man!");
    const long double testValue = std::numeric_limits<long double>::max() / 2;
    long double value = cell.ToLongDoubleDef(testValue);
    QCOMPARE(value, testValue);
}

void CsvGridTest::Case80_Cell_LongToDoubleDef_2()
{
    const long double testValue = std::numeric_limits<long double>::max() / 2;
    core_lib::csv_grid::Cell cell(testValue);
    long double value = cell.ToLongDoubleDef(0);
    QCOMPARE(value, testValue);
}

void CsvGridTest::Case81_Row_InitializerListLongDoubleConstructor()
{
    long double i = 0;
    core_lib::csv_grid::Row row = { Cell(i), Cell(i), Cell(i), Cell(i), Cell(i) };
    QCOMPARE(row.GetSize(), static_cast<size_t>(5));
}

void CsvGridTest::Case82_Row_AddColumnAsLongDouble()
{
    core_lib::csv_grid::Row row;
    QCOMPARE(row.GetSize(), static_cast<size_t>(0));
    row.AddColumn(100.1L);
    QCOMPARE(row.GetSize(), static_cast<size_t>(1));
    QCOMPARE(static_cast<long double>(row[0]), 100.1L);
}

void CsvGridTest::Case83_Row_InsertColumnAsLongDouble()
{
    core_lib::csv_grid::Row row = { Cell(1.1L), Cell(2.2L), Cell(3.3L), Cell(4.4L), Cell(5.5L) };
    row.InsertColumn(3, 666.6L);
    QCOMPARE(row.GetSize(), static_cast<size_t>(6));
    QCOMPARE(static_cast<long double>(row[0]), 1.1L);
    QCOMPARE(static_cast<long double>(row[1]), 2.2L);
    QCOMPARE(static_cast<long double>(row[2]), 3.3L);
    QCOMPARE(static_cast<long double>(row[3]), 666.6L);
    QCOMPARE(static_cast<long double>(row[4]), 4.4L);
    QCOMPARE(static_cast<long double>(row[5]), 5.5L);
}

QTEST_APPLESS_MAIN(CsvGridTest)

#include "tst_CsvGridTest.moc"
