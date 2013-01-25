#include "multimediascontainerscriptclass.h"

#include "multimediascontainer.h"

Q_DECLARE_METATYPE(MultiMediasContainer)
Q_DECLARE_METATYPE(MultiMediasContainer*)
Q_DECLARE_METATYPE(MultiMediasContainerScriptClass*)

// MultiMediasContainerScriptPrototype class

MultiMediasContainerScriptPrototype::MultiMediasContainerScriptPrototype(QObject *parent) : QObject(parent)
{
}

MultiMediasContainerScriptPrototype::~MultiMediasContainerScriptPrototype()
{
}

MultiMediasContainer *MultiMediasContainerScriptPrototype::thisMultiMediasContainer() const
{
	return qscriptvalue_cast<MultiMediasContainer*>(thisObject().data());
}

void MultiMediasContainerScriptPrototype::askItemsToDownload()
{
	thisMultiMediasContainer()->askItemsToDownload();
}

void MultiMediasContainerScriptPrototype::addMediaURL(QString title, QString url)
{
	thisMultiMediasContainer()->addMediaURL(title, url);
}

// MultiMediasContainerScriptClass class

MultiMediasContainerScriptClass::MultiMediasContainerScriptClass(QScriptEngine *engine)
	: QObject(engine), QScriptClass(engine)
{
	qScriptRegisterMetaType<MultiMediasContainer>(engine, toScriptValue, fromScriptValue);

	proto = engine->newQObject(new MultiMediasContainerScriptPrototype(this),
							   QScriptEngine::QtOwnership,
							   QScriptEngine::SkipMethodsInEnumeration
							   | QScriptEngine::ExcludeSuperClassMethods
							   | QScriptEngine::ExcludeSuperClassProperties);
	QScriptValue global = engine->globalObject();
	proto.setPrototype(global.property("Object").property("prototype"));

	ctor = engine->newFunction(construct);
	ctor.setData(qScriptValueFromValue(engine, this));

	engine->globalObject().setProperty("MultiMediasContainer", this->constructor());
}

MultiMediasContainerScriptClass::~MultiMediasContainerScriptClass()
{
}

QScriptValue MultiMediasContainerScriptClass::construct(QScriptContext *ctx, QScriptEngine *)
{
	MultiMediasContainerScriptClass *cls = qscriptvalue_cast<MultiMediasContainerScriptClass*>(ctx->callee().data());
	if (!cls) return QScriptValue();
	return cls->newInstance();
}

QScriptValue MultiMediasContainerScriptClass::toScriptValue(QScriptEngine *eng, const MultiMediasContainer &multiMediasContainer)
{
	QScriptValue ctor = eng->globalObject().property("MultiMediasContainer");
	MultiMediasContainerScriptClass *cls = qscriptvalue_cast<MultiMediasContainerScriptClass*>(ctor.data());
	if (!cls) return eng->newVariant(qVariantFromValue(multiMediasContainer));
	return cls->newInstance(multiMediasContainer);
}

void MultiMediasContainerScriptClass::fromScriptValue(const QScriptValue &obj, MultiMediasContainer &multiMediasContainer)
{
	multiMediasContainer = qscriptvalue_cast<MultiMediasContainer>(obj.data());
}

QScriptValue MultiMediasContainerScriptClass::constructor()
{
	return ctor;
}

QString MultiMediasContainerScriptClass::name() const
{
	return QLatin1String("MultiMediasContainer");
}

QScriptValue MultiMediasContainerScriptClass::prototype() const
{
	return proto;
}

QScriptValue::PropertyFlags MultiMediasContainerScriptClass::propertyFlags(const QScriptValue&, const QScriptString&, uint)
{
	return QScriptValue::Undeletable;
}

QScriptValue MultiMediasContainerScriptClass::newInstance()
{
	return newInstance(MultiMediasContainer());
}

QScriptValue MultiMediasContainerScriptClass::newInstance(const MultiMediasContainer &multiMediasContainer)
{
	QScriptValue data = engine()->newVariant(qVariantFromValue(multiMediasContainer));
	return engine()->newObject(this, data);
}
