#include "standardmessageoutpipe.h"
#include "contacts/imcontact.h"

namespace SIM
{
    StandardMessageOutPipe::StandardMessageOutPipe()
    {

    }

    StandardMessageOutPipe::~StandardMessageOutPipe()
    {

    }

    void StandardMessageOutPipe::pushMessage(const MessagePtr& message)
    {
        foreach(MessageProcessor* processor, m_processors)
        {
            if(processor->process(message) == MessageProcessor::Block)
                return;
        }
        IMContactPtr contact = message->targetContact().toStrongRef();
        if(!contact)
            return;
        contact->sendMessage(message);
    }

    void StandardMessageOutPipe::addMessageProcessor(MessageProcessor* processor)
    {
        m_processors.append(processor);
    }

    void StandardMessageOutPipe::removeMessageProcessor(const QString& id) //Todo
    {

    }

    void StandardMessageOutPipe::setContactList(ContactList* contactList)
    {
        m_contactList = contactList;
    }
}
