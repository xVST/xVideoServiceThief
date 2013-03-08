#ifndef MULTIMEDIASCONTAINERSCRIPTCLASS_H
#define MULTIMEDIASCONTAINERSCRIPTCLASS_H

#include <QtCore>
#include <QtScript>

class MultiMediasContainer;

class MultiMediasContainerScriptPrototype : public QObject, public QScriptable
{
Q_OBJECT
	private:
		MultiMediasContainer *thisMultiMediasContainer() const;
	public:
		MultiMediasContainerScriptPrototype(QObject *parent = 0);
		~MultiMediasContainerScriptPrototype();
	public slots:
		void askItemsToDownload();
		void addMediaURL(QString title, QString url);
};

class MultiMediasContainerScriptClass : public QObject, public QScriptClass
{
Q_OBJECT
	private:
		QScriptValue proto;
		QScriptValue ctor;
		static QScriptValue construct(QScriptContext *ctx, QScriptEngine *eng);
		static QScriptValue toScriptValue(QScriptEngine *eng, const MultiMediasContainer &multiMediasContainer);
		static void fromScriptValue(const QScriptValue &obj, MultiMediasContainer &multiMediasContainer);
	public:
		MultiMediasContainerScriptClass(QScriptEngine *engine);
		~MultiMediasContainerScriptClass();
		QScriptValue constructor();
		QString name() const;
		QScriptValue prototype() const;
		QScriptValue::PropertyFlags propertyFlags(const QScriptValue &object, const QScriptString &name, uint id);
		QScriptValue newInstance();
		QScriptValue newInstance(const MultiMediasContainer &multiMediasContainer);
};

#endif // MULTIMEDIASCONTAINERSCRIPTCLASS_H
