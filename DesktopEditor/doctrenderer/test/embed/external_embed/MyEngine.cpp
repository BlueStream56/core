#include "MyEngine.h"

#include <iostream>

// this file is a v8_base.cpp (and may be jsc_base.mm) imitation

CEmbedObject::CEmbedObject() : m_embed_internal(new CEmbedObjectPrivate(this))
{
}

CEmbedObject::~CEmbedObject()
{
}

void CEmbedObjectPrivate::Embed()
{
	m_sResult << "METHODS EMBEDED:" << std::endl;
	m_pOwner->EmbedMethods();
	std::cout << m_sResult.str();
}

void CMyEngine::Embed(CEmbedObject* object, const std::string& name)
{
	std::cout << name << std::endl;
	object->m_embed_internal->Embed();
}
