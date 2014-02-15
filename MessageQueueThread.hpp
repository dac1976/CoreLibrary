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
     * The decoder function should not throw any exceptions.
     */
    typedef std::function< MessageId (const MessageType*, size_t length) > msg_id_decoder;
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
     * The handler function should not throw any exceptions.
     */
    typedef std::function< bool (MessageType*, size_t length) > msg_handler;
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
     * \brief Put a message onto this threads queue.
     * \param [IN] Pointer to message.
     * \param [IN] Number of objects of type MessageType pointed to by msg.
     */
    void PutMessage(MessageType* msg, size_t length)
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
        size_t length;
        MessageType* msg = m_messageQueue.Pop(length);

        if (msg && (length > 0))
        {
            bool deleteMsg = true;
            MessageId messageId = m_msgIdDecoder(msg, length);

            if (m_msgHandlerMap.count(messageId) > 0)
            {
                deleteMsg = m_msgHandlerMap[messageId](msg, length);
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
        m_messageQueue.PushNull();
    }
    /*!
     * \brief Delete a processed message.
     * \param [IN] Pointer to message.
     * \param [IN] Number of objects of type MessageType pointed to by msg.
     */
    static void DeleteMessage(const MessageType* msg, size_t length)
    {
        if (msg)
        {
            if (length > 1)
                delete [] msg;
            else
                delete msg;
        }
    }
};

} // namespace threads
} // namespace core_lib

#endif // MESSAGEQUEUETHREAD_HPP
