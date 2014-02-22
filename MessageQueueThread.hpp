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
     * \param [IN] A user specifed message string.
     */
    explicit xMsgHandlerError(const std::string& message);
    /*! \brief Virtual destructor. */
    virtual ~xMsgHandlerError();
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
 * queueOptions - define the queue options to be used with the
 *                underlying ConcurrentQueue. Set multiple options
 *                using | operator.
 *                See enum eConcurrentQueueOptions in ConcurrentQueue.hpp.
 */
template< typename MessageId
          , typename MessageType
          , eQueueOptions queueOptions = eQueueOptions::autoDelete >
class MessageQueueThread final : public ThreadBase
{
public:
    /*!
     * \brief Typedef defining message ID decoder function.
     * \param [IN] Pointer to message.
     * \param [IN] Number of objects of type MessageType pointed to by msg.
     * \return Unique ID of the message to be processed.
     *
     * The decoder function should not throw any exceptions. If the message
     * is a signel item and not and array of items of type MessageType
     * the the length will be the special value of -1.
     */
    typedef std::function< MessageId (const MessageType*, int length) > msg_id_decoder;
    /*!
     * \brief Default constructor.
     * \param [IN] Function object that returns the message ID for a message.
     */
    explicit MessageQueueThread(const msg_id_decoder& messageIdDecoder)
        : ThreadBase()
        , m_msgIdDecoder(messageIdDecoder)
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
    }
    /*!
     * \brief Typedef defining message handler function.
     * \param [IN] Pointer to message.
     * \param [IN] Number of objects of type MessageType pointed to by msg.
     * \return True if message can be deleted, false otherwise.
     *
     * The decoder function should not throw any exceptions. If the message
     * is a signel item and not and array of items of type MessageType
     * the the length will be the special value of -1.
     */
    typedef std::function< bool (MessageType*, int length) > msg_handler;
    /*!
     * \brief Register a function to handle a particular message.
     * \param [IN] Message ID.
     * \param [IN] Function object to handle messages with specified message ID.
     *
     * Throws a xMsgHandlerError exception if handler for message ID is
     * already defined.
     */
    void RegisterMessageHandler(MessageId messageID
                                , const msg_handler& messageHandler)
    {
        if (m_msgHandlerMap.count(messageID) > 0)
            BOOST_THROW_EXCEPTION(xMsgHandlerError("message handler already defined"));

        m_msgHandlerMap[messageID] = messageHandler;
    }
    /*!
     * \brief Push a message onto this thread's queue.
     * \param [IN] Pointer to message.
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
     * \param [IN] Pointer to message.
     * \param [IN] Number of objects of type MessageType pointed to by msg.
     *
     * Messages pushed on using this function will be deleted
     * with delete[] if length > 0.
     */
    void Push(MessageType* msg, int length)
    {
        m_messageQueue.Push(msg, length);
    }

private:
    /*! \brief Message ID decoder function object. */
    msg_id_decoder m_msgIdDecoder;
    typedef std::map< MessageId, msg_handler > msg_map;
    /*! \brief Message handler function Map. */
    msg_map m_msgHandlerMap;
    /*! \brief Message handler Map. */
    ConcurrentQueue< MessageType, queueOptions > m_messageQueue;

    /*! \brief Execute a single iteration of the thread. */
    virtual void ThreadIteration()
    {
        int length;
        MessageType* msg = m_messageQueue.Pop(&length);

        if (msg && (length != 0))
        {
            bool deleteMsg;

            try
            {
                MessageId messageId = m_msgIdDecoder(msg, length);

                if (m_msgHandlerMap.count(messageId) > 0)
                {
                    deleteMsg = m_msgHandlerMap[messageId](msg, length);
                }
                else
                {
                    deleteMsg = queueOptions == eQueueOptions::autoDelete;
                }
            }
            catch(...)
            {
                deleteMsg = queueOptions == eQueueOptions::autoDelete;
            }

            if (deleteMsg)
            {
                DeleteMessage(msg, length);
            }
        }
    }
    /*! \brief Perform any special termination actions.*/
    virtual void ProcessTerminationConditions()
    {
        // Make sure we break out of m_messageQueue.Pop();
        m_messageQueue.Push();
    }
    /*!
     * \brief Delete a processed message.
     * \param [IN] Pointer to message.
     * \param [IN] Number of objects of type MessageType pointed to by msg.
     */
    void DeleteMessage(const MessageType* msg, int length)
    {
        if (msg)
        {
            if (length > 0)
                delete [] msg;
            else
                delete msg;
        }
    }
};

} // namespace threads
} // namespace core_lib

#endif // MESSAGEQUEUETHREAD_HPP
