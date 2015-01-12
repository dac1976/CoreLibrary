// This file is part of CoreLibrary containing useful reusable utility
// classes.
//
// Copyright (C) 2014 Duncan Crutchley
// Contact <duncan.crutchley+corelibrary@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License and GNU Lesser General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License
// and GNU Lesser General Public License along with this program. If
// not, see <http://www.gnu.org/licenses/>.


/*!
 * \file MessageQueueThread.hpp
 * \brief File containing declaration of MessageQueueThread class.
 */

#ifndef MESSAGEQUEUETHREAD_HPP
#define MESSAGEQUEUETHREAD_HPP

#include <functional>
#include <map>
#include "ThreadBase.hpp"
#include "ConcurrentQueue.hpp"

/*! \brief The core_lib namespace. */
namespace core_lib {
/*! \brief The threads namespace. */
namespace threads {

/*!
 * \brief Message handler exception.
 *
 * This exception class is intended to be thrown by functions in
 * MessageQueueThread class when a message handler error occurs.
 */
class xMsgHandlerError : public exceptions::xCustomException
{
public:
	/*! \brief Default constructor. */
	xMsgHandlerError();
	/*!
	 * \brief Initializing constructor.
	 * \param[in] message - A user specified message string.
	 */
	explicit xMsgHandlerError(const std::string& message);
	/*! \brief Virtual destructor. */
	virtual ~xMsgHandlerError();
	/*! \brief Copy constructor. */
	xMsgHandlerError(const xMsgHandlerError&) = default;
	/*! \brief Move constructor. */
	xMsgHandlerError(xMsgHandlerError&&) = default;
	/*! \brief Copy assignment operator. */
	xMsgHandlerError& operator=(const xMsgHandlerError&) = default;
	/*! \brief Move assignment operator. */
	xMsgHandlerError& operator=(xMsgHandlerError&&) = default;
};
/*! \brief Control how messages get destroyed in destructor. */
enum class eOnDestroyOptions
{
	/*! Delete remaining items on desruction. */
	deleteRemainingItems,
	/*! Process remaining items on desruction. */
	processRemainingItems
};
/*!
 * \brief Message Queue Thread.
 *
 * This class is designed for processing messages specified
 * by the template argument. The class itself contains no knowledge
 * of how to process these messages, this is handled by the user
 * registering message handler functions. Instead this class provides
 * a safe thread based mechanism for processing messages added to this
 * classes message queue.
 *
 * Template args as follows:
 * MessageId    - define the type used for message ID.
 * MessageType  - define the type used for the message.
 */
template<typename MessageId, typename MessageType>
class MessageQueueThread final : public ThreadBase
{
public:
	/*!
	 * \brief Typedef defining message ID decoder function.
	 * \param[in] message - Pointer to message.
	 * \param[in] count - Number of objects of type MessageType pointed to by msg.
	 * \return Unique ID of the message to be processed.
	 *
	 * The decoder function should not throw any exceptions. If the message
	 * is a signel item and not and array of items of type MessageType
	 * the the length will be the special value of -1.
	 */
	typedef std::function<MessageId (const MessageType*, const int )> msg_id_decoder;
	/*!
	 * \brief Default constructor.
	 * \param[in] messageIdDecoder - Function object that returns the message ID for a message.
	 * \param[in] destroyOptions - (Optional) Set the Message threads destroy option.
	 * \param[in] queueOptions - (Optional) Set the queue's delete option.
	 */
	explicit MessageQueueThread(msg_id_decoder messageIdDecoder
								, eOnDestroyOptions destroyOptions
								= eOnDestroyOptions::deleteRemainingItems
								  , eQueueOptions queueOptions
								= eQueueOptions::autoDelete)
		: ThreadBase()
		, m_msgIdDecoder{std::move(messageIdDecoder)}
		, m_destroyOptions{destroyOptions}
		, m_queueOptions{queueOptions}
		, m_messageQueue{queueOptions}
	{
		Start();
	}
	/*! \brief Copy constructor deleted.*/
	MessageQueueThread(const MessageQueueThread&) = delete;
	/*! \brief Copy assignment operator deleted.*/
	MessageQueueThread& operator=(const MessageQueueThread&) = delete;
	/*! \brief Destructor.*/
	virtual ~MessageQueueThread()
	{
		Stop();

		if (m_destroyOptions == eOnDestroyOptions::processRemainingItems)
		{
			while (!m_messageQueue.Empty())
			{
				ProcessNextMessage();
			}
		}
	}
	/*!
	 * \brief Typedef defining message handler function.
	 * \param[in] message - Pointer to message.
	 * \param[in] count - Number of objects of type MessageType pointed to by msg.
	 * \return True if message can be deleted, false otherwise.
	 *
	 * The decoder function should not throw any exceptions. If the message
	 * is a signel item and not and array of items of type MessageType
	 * the the length will be the special value of -1.
	 */
	typedef std::function<bool (MessageType*, const int )> msg_handler;
	/*!
	 * \brief Register a function to handle a particular message.
	 * \param[in] messageID - Message ID.
	 * \param[in] messageHandler - Function object to handle messages with specified message ID.
	 *
	 * Throws a xMsgHandlerError exception if handler for message ID is
	 * already defined.
	 */
	void RegisterMessageHandler(const MessageId messageID
								, msg_handler messageHandler)
	{
		std::lock_guard<std::mutex> lock{m_mutex};

		if (m_msgHandlerMap.count(messageID) > 0)
		{
			BOOST_THROW_EXCEPTION(xMsgHandlerError("message handler already defined"));
		}

		m_msgHandlerMap.emplace(messageID, std::move(messageHandler));
	}
	/*!
	 * \brief Push a message onto this thread's queue.
	 * \param[in] msg - Pointer to message.
	 *
	 * Messages pushed on using this function will be deleted
	 * with delete.
	 */
	void Push(MessageType* msg)
	{
		m_messageQueue.Push(msg);
	}

	/*!
	 * \brief Push a message as an array of items onto this thread's queue.
	 * \param[in] msg - Pointer to message.
	 * \param[in] length - Number of objects of type MessageType pointed to by msg.
	 *
	 * Messages pushed on using this function will be deleted
	 * with delete[] if length > 0.
	 */
	void Push(MessageType* msg, const int length)
	{
		m_messageQueue.Push(msg, length);
	}

private:
	/*! Mutex to lock access to message handler map. */
	mutable std::mutex m_mutex;
	/*! \brief Message ID decoder function object. */
	msg_id_decoder m_msgIdDecoder;
	/*! \brief Control the destruction of the queue items. */
	const eOnDestroyOptions m_destroyOptions;
	/*! \brief Queue option to copntrol how items are deleted. */
	const eQueueOptions m_queueOptions{eQueueOptions::autoDelete};
	/*! \brief Typedef for message map type. */
	typedef std::map<MessageId, msg_handler> msg_map;
	/*! \brief Message handler function Map. */
	msg_map m_msgHandlerMap;
	/*! \brief Message queue. */
	ConcurrentQueue<MessageType> m_messageQueue;

	/*! \brief Execute a single iteration of the thread. */
	virtual void ThreadIteration()
	{
		ProcessNextMessage();
	}
	/*! \brief Perform any special termination actions.*/
	virtual void ProcessTerminationConditions()
	{
		// Make sure we break out of m_messageQueue.Pop();
		m_messageQueue.Push();
	}
	/*!
	 * \brief Process next message.
	 */
	void ProcessNextMessage()
	{
		int length;
		MessageType* msg = m_messageQueue.Pop(&length);

		if (msg && (length != 0))
		{
			bool deleteMsg;

			try
			{
				MessageId messageId{m_msgIdDecoder(msg, length)};
				std::lock_guard<std::mutex> lock{m_mutex};

				if (m_msgHandlerMap.count(messageId) > 0)
				{
					deleteMsg = m_msgHandlerMap[messageId](msg, length);
				}
				else
				{
					deleteMsg = m_queueOptions == eQueueOptions::autoDelete;
				}
			}
			catch(...)
			{
				deleteMsg = m_queueOptions == eQueueOptions::autoDelete;
			}

			if (deleteMsg)
			{
				if (length > 0)
				{
					delete [] msg;
				}
				else
				{
					delete msg;
				}
			}
		}
	}
};

} // namespace threads
} // namespace core_lib

#endif // MESSAGEQUEUETHREAD_HPP
