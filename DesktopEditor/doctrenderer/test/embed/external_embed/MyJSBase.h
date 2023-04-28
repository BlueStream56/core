#ifndef MYJSBASE_H
#define MYJSBASE_H

// this file is js_base.h imitation

#include <string>

// JSSmart<CJSValue> imitation
using ValueType = int;

class CEmbedObjectPrivate;
// CJSEmbededObject imitation
class CEmbedObject
{
public:
	CEmbedObject();
	virtual ~CEmbedObject();

public:
	virtual void EmbedMethods() = 0;
	virtual void* getObject() = 0;

public:
	CEmbedObjectPrivate* m_embed_internal;

	friend class CEmbedObjectPrivate;
};

// CJSContext imitation
class CMyEngine
{
public:
	CMyEngine() = default;
	~CMyEngine() = default;

	void Embed(CEmbedObject* object, const std::string& name);
};

#endif // MYJSBASE_H
