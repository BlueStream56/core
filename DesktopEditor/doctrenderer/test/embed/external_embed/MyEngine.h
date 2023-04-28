#ifndef MYENGINE_H
#define MYENGINE_H

// this file is a v8_base.h (or jsc_base.h) imitation

#include "MyJSBase.h"

#include <sstream>

//#define EMBED_FUNCTION()

class CEmbedObjectPrivate
{
	// templates (aka wrappers)
public:
	template <class EmbedClass>
	static ValueType func(ValueType(EmbedClass::*F)(), EmbedClass* pEmbed)
	{
		return (pEmbed->*F)();
	}

	template <class EmbedClass>
	static ValueType func(ValueType(EmbedClass::*F)(ValueType), EmbedClass* pEmbed)
	{
		return (pEmbed->*F)(10);
	}

	template <class EmbedClass>
	static ValueType func(ValueType(EmbedClass::*F)(ValueType, ValueType), EmbedClass* pEmbed)
	{
		return (pEmbed->*F)(42, 21);
	}

	// own methods
public:
	void Embed();

	// overloaded methods
public:
	CEmbedObjectPrivate(CEmbedObject* owner) : m_pOwner(owner) {}
	virtual ~CEmbedObjectPrivate() = default;

public:
	std::stringstream m_sResult;

private:
	CEmbedObject* m_pOwner;
};

#endif // MYENGINE_H
