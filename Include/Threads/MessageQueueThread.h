// This file is part of CoreLibrary containing useful reusable utility
// classes.
//
// Copyright (C) 2014 to present, Duncan Crutchley
// Contact <dac1976github@outlook.com>
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
 * \file MessageQueueThread.h
 * \brief File containing declaration of MessageQueueThread class.
 */

#ifndef MESSAGEQUEUETHREAD
#define MESSAGEQUEUETHREAD

#include "Platform/PlatformDefines.h"
#include <functional>
#include <map>
#include "ThreadBase.h"
#include "ConcurrentQueue.h"

/*! \brief The core_lib namespace. */
namespace core_lib
{
/*! \brief The threads namespace. */
namespace threads
{

/*!
 * \brief Message handler exception.
 *
 * This exception class is intended to be thrown by functions in
 * MessageQueueThread class when a message handler error occurs.
 */
class CORE_LIBRARY_DLL_SHARED_API xMsgHandlerError : public exceptions::xCustomException
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
    /*! \brief Copy assignment operator. */
    xMsgHandlerError& operator=(const xMsgHandlerError&) = default;
};
/*! \brief Control how messages get destroyed in destructor. */
enum class eOnDestroyOptions
{
    /*! Ignore remaining items. */
    ignoreRemainingItems,
    /*! Process remaining items. */
    processRemainingItems
};
/*!
 * \brief Message Queue Thread.
 *
 * This class is designed for processing messages specified
 * by the template argument. The class itself contains no knowledge
 * of how to process these messages, this is handled by the user
 * registering message handler functors per registered command. Instead
 * this class provides a safe thread based mechanism for processing
 * messages added to this class's message queue.
 *
 * Template args as follows:
 * MessageId    - define the type used for message ID.
 * MessageType  - define the type used for the message.
 */
template <typename MessageId, typename MessageType>
class MessageQueueThread final : public ThreadBase
{
    /*!
     * \brief Typedef defining message ID decoder function.
     * \param[in] message - Const reference to message.
     * \return Unique ID of the message to be processed.
     *
     * The decoder function should not throw any exceptions.
     */
    typedef std::function<MessageId(const MessageType&)> msg_id_decoder_t;
    /*!
     * \brief Typedef defining message deleter function.
     * \param[in] message - Reference to message.
     *
     * The deleter function should not throw any exceptions.
     */
    typedef std::function<void(MessageType&)> msg_deleter_t;

public:
    /*!
     * \brief Default constructor.
     * \param[in] messageIdDecoder - Function object that returns the message ID for a message.
     * \param[in] destroyOptions - (Optional) Set the Message threads destroy option.
     * \param[in] messageDeleter - (Optional) Message deletion helper.
     */
    explicit MessageQueueThread(
        const msg_id_decoder_t& messageIdDecoder,
        eOnDestroyOptions       destroyOptions = eOnDestroyOptions::ignoreRemainingItems,
        const msg_deleter_t&    messageDeleter = msg_deleter_t())
        : ThreadBase()
        , m_msgIdDecoder{messageIdDecoder}
        , m_destroyOptions{destroyOptions}
        , m_messageDeleter{messageDeleter}
    {
        if (!Start())
        {
            BOOST_THROW_EXCEPTION(xThreadNotStartedError("ThreadBase::Start() returned false"));
        }
    }
    /*! \brief Copy constructor deleted.*/
    MessageQueueThread(const MessageQueueThread&) = delete;
    /*! \brief Copy assignment operator deleted.*/
    MessageQueueThread& operator=(const MessageQueueThread&) = delete;
    /*! \brief Destructor.*/
    virtual ~MessageQueueThread()
    {
        Stop();

        while (!m_messageQueue.Empty())
        {
            switch (m_destroyOptions)
            {
            case eOnDestroyOptions::processRemainingItems:
                ProcessNextMessage();
                break;
            case eOnDestroyOptions::ignoreRemainingItems:
            default:
                DeleteNextMessage();
                break;
            }
        }
    }
    /*!
     * \brief Typedef defining message handler functor.
     * \param[in] message - Reference to message.
     * \return True if message is finished with, false if message ownership has taken by another
     * object.
     *
     * The decoder function should not throw any exceptions.
     */
    typedef std::function<bool(MessageType&)> msg_handler_t;
    /*!
     * \brief Register a function to handle a particular message.
     * \param[in] messageID - Message ID.
     * \param[in] messageHandler - Function object to handle messages with specified message ID.
     *
     * Throws a xMsgHandlerError exception if handler for message ID is
     * already defined.
     */
    void RegisterMessageHandler(const MessageId messageID, const msg_handler_t& messageHandler)
    {
        std::lock_guard<std::mutex> lock{m_mutex};

        if (m_msgHandlerMap.count(messageID) > 0)
        {
            BOOST_THROW_EXCEPTION(xMsgHandlerError("message handler already defined"));
        }

        m_msgHandlerMap.emplace(messageID, messageHandler);
    }
    /*!
     * \brief Push a message as an array of items onto this thread's queue.
     * \param[in] msg - message, perfectly forwarded.
     */
    void Push(MessageType&& msg)
    {
        m_messageQueue.Push(std::forward<MessageType>(msg));
    }

private:
    /*! Mutex to lock access to message handler map. */
    mutable std::mutex m_mutex;
    /*! \brief Message ID decoder function object. */
    msg_id_decoder_t m_msgIdDecoder;
    /*! \brief Control the handling of the queue items in destructor. */
    const eOnDestroyOptions m_destroyOptions;
    /*! \brief Optional message item deleter function object. */
    msg_deleter_t m_messageDeleter;
    /*! \brief Typedef for message map type. */
    typedef std::map<MessageId, msg_handler_t> msg_map_t;
    /*! \brief Message handler function Map. */
    msg_map_t m_msgHandlerMap;
    /*! \brief Message queue. */
    ConcurrentQueue<MessageType> m_messageQueue;

    /*! \brief Execute a single iteration of the thread. */
    virtual void ThreadIteration() NO_EXCEPT_
    {
        ProcessNextMessage();
    }
    /*! \brief Perform any special termination actions.*/
    virtual void ProcessTerminationConditions() NO_EXCEPT_
    {
        // Make sure we break out of m_messageQueue.Pop();
        m_messageQueue.BreakPopWait();
    }
    /*! \brief Process next message. */
    void ProcessNextMessage()
    {
        MessageType msg{};

        if (!m_messageQueue.Pop(msg))
        {
            return;
        }

        bool canDeleteMsg;

        try
        {
            MessageId                   messageId{m_msgIdDecoder(msg)};
            std::lock_guard<std::mutex> lock{m_mutex};

            if (m_msgHandlerMap.count(messageId) > 0)
            {
                canDeleteMsg = m_msgHandlerMap[messageId](msg);
            }
            else
            {
                canDeleteMsg = true;
            }
        }
        catch (...)
        {
            canDeleteMsg = true;
        }

        if (canDeleteMsg && m_messageDeleter)
        {
            try
            {
                m_messageDeleter(msg);
            }
            catch (...)
            {
                // Do nothing.
            }
        }
    }
    /*! \brief Delete next message. */
    void DeleteNextMessage()
    {
        if (!m_messageDeleter)
        {
            return;
        }

        MessageType msg{};

        if (!m_messageQueue.Pop(msg))
        {
            return;
        }

        try
        {
            m_messageDeleter(msg);
        }
        catch (...)
        {
            // Do nothing.
        }
    }
};

} // namespace threads
} // namespace core_lib

#endif // MESSAGEQUEUETHREAD
