#ifndef QF_CORE_UTILS_SVALUE_H
#define QF_CORE_UTILS_SVALUE_H

#include "../core/coreglobal.h"

#include <QSharedData>
#include <QVariant>
#include <QJsonDocument>

namespace qf {
namespace core {
namespace utils {

/**
 * Universal explicitly shared value
 *
 * It is a wrapper around QVariant, which makes it explicitly shared and adds some methods
 * that allows to use it as a list or map of properties.  Lists and maps can be nested
 * to create more complicated structures of properties.
 */
class QFCORE_DECL_EXPORT SValue
{
public:
	static const QString FLD_VALUE;
	static const QString FLD_META;
public:
	/**
	 * Creates shared value with no data (invalid value)
	 */
	SValue();

	/**
	 * Creates shared value from given variant
	 *
	 * If the variant carries the SValue, new SValue will just reference the given one (no copy is made).
	 * If the variant carries any other type, new SValue will be created with copy of the given one.
	 *
	 * @param v
	 */
	SValue(const QVariant &v);

	/**
	 * Destructor just decreases reference counter for the shared data
	 */
	virtual ~SValue();
private:
	struct Data : QSharedData {
		QVariant value;
	};
	QExplicitlySharedDataPointer<Data> d;
protected:
	/**
	 * Treats the stored value as a QString->SValue map and gets value on key @p name
	 *
	 * If stored value is not a map, it is replaced with a map and stores current value
	 * to the key "value".
	 *
	 * If the value of the key @p name is not SValue, it is replaced with the SValue
	 * created from current value.
	 *
	 * If the key @p is not in the map, it is set to invalid SValue.
	 *
	 * @param name Key to be found in the map
	 * @return SValue referencing value on key @p name.
	 */
	SValue property_helper(const QString &name);


	/**
	 * Treats the stored value as a SValue list and gets value on index @p ix
	 *
	 * If stored value is not a list, it is replaced with a list that has all
	 * indexes up to @p ix filled with invalid SValue.
	 *
	 * If value on index @p ix is not SValue, it is replaced with the SValue
	 * created from current value.
	 *
	 * If the list has less items than @p ix, it is filled with invalid SValues
	 * up to the index @p ix.
	 *
	 * @param ix Index of the item to be retrieved
	 * @return SValue referencing value on index @p ix.
	 */
	SValue property_helper(int ix);
public:
	/**
	 * Checks, if it has some value.
	 */
	bool isValid() const {return d->value.isValid();}

	/**
	 * Gets type of stored value
	 */
	QVariant::Type valueType() const {return d->value.type();}

	/**
	 * Checks, if stored value is a list of SValues
	 */
	bool isList() const {return valueType() == QVariant::List;}

	/**
	 * Checks, if stored value is a QString->SValue map
	 */
	bool isMap() const {return valueType() == QVariant::Map;}

	/**
	 * Treats the stored value as a map and tries to get value of key @p name.
	 *
	 * @param name Name of the key(property) to be found.
	 * @param[out] val If given, it will be filled with key value. It will be invalidated,
	 * 		if the stored value is not a map or if the key doesn't exist.
	 * @return True, if the stored value is the map and the key @p name exists.
	 */
	bool hasProperty(const QString &name, QVariant *val = NULL) const;

	/**
	 * Treats the stored value as a map and lists all of its keys
	 *
	 * @return String list of all keys or empty list if stored value is not a map.
	 */
	QStringList keys() const;

	/**
	 * Treats the stored value as a list and counts number of items.
	 *
	 * @return Number of items in the list or 0 if stored value is not a list.
	 */
	int count() const {return isList()? d->value.toList().count(): 0;}

	/**
	 * @return Shallow copy of value referenced by this SValue.
	 */
	QVariant value() const;

	/**
	 * Sets the value to the shallow copy of the given one
	 *
	 * @param val Value to be set.
	 */
	void setValue(const QVariant &val);

	/**
	 * Sets the value to the shallow copy of the given one
	 *
	 * @param val Value to be set.
	 */
	void setValue(const SValue &sval);

	/**
	 * Treats the stored value as a map and gets value of key @p name
	 *
	 * @param name Key(property) to be retrieved from the map.
	 * @param default_value Value to be returned key doesn't exist in the map.
	 * @return Value of the key @p name or @p default_value.
	 */
	QVariant property(const QString &name, const QVariant &default_value = QVariant()) const;

	/**
	 * Treats the stored value as a map tree and goes along a @p path to get a value.
	 *
	 * @param path Path to a value in a map tree
	 * @param default_value Value to be returned if value on the path doesn't exist.
	 * @return Value at the given @p path or @p default_value
	 */
	QVariant propertyOnPath(const QStringList &path, const QVariant &default_value = QVariant()) const;

	QVariant propertyOnPath(const QString &path, const QVariant &default_value = QVariant()) const {return propertyOnPath(path.split('.'), default_value);}
	/// pokud je vracena property typu mapa a ta obsahuje field value, bere se on
	QVariant propertyValueOnPath(const QStringList &path, const QVariant &default_value = QVariant()) const;
	QVariant propertyValueOnPath(const QString &path, const QVariant &default_value = QVariant()) const {return propertyValueOnPath(path.split('.'), default_value);}

	/**
	 * Treats the stored value as a map and gets value of key @p name
	 *
	 * If the value of the key is of type SValue, reference to it is returned.
	 * If the value of the key is of any other type, shallow copy of it is returned.
	 *
	 * @param name Key(property) to be retrieved from the map.
	 * @return SValue referencing or copying value of the key @p name.
	 */
	SValue property2(const QString &name) const {return SValue(property(name));}

	/**
	 * Treats the stored value as a list and gets value at indes @p ix
	 *
	 * If the value at the index is of type SValue, reference to it is returned.
	 * If the value at the index is of any other type, shallow copy of it is returned.
	 *
	 * @param ix Index of item to be retrieved from the list.
	 * @return SValue referencing or copying value of the item at index @p name.
	 */
	SValue property2(int ix) const {return SValue(property(ix));}

	/**
	 * Treats the stored value as a map and sets value of key @p name to value @p val
	 *
	 * If stored value is not a map, it is replaced with a map and stores current value
	 * to the key "value".
	 *
	 * If the @p val is not a SValue, it is converted to it. If it is list or map,
	 * conversion is done recursively.
	 *
	 * @param name Name of the key to be set
	 * @param val New value
	 */
	void setProperty(const QString &name, const QVariant &val);

	/**
	 * Treats the stored value as a map and sets value of key @p name to value @p val
	 *
	 * If stored value is not a map, it is replaced with a map and stores current value
	 * to the key "value".
	 *
	 * @param name Name of the key to be set
	 * @param val New value
	 */
	void setProperty(const QString &name, const SValue &val) {QVariant v; v.setValue(val); setProperty(name, v);}

	/**
	 * Treats the stored value as a list and gets value at indes @p ix
	 *
	 * @param ix Index of item to be retrieved from the list.
	 * @param default_value Value to be returned if value on the index @p ix doesn't exist.
	 * @return Value at the given index or @p default_value
	 */
	QVariant property(int ix, const QVariant &default_value = QVariant()) const;

	/**
	 * Treats the stored value as a list and sets value at index @p ix to value @p val
	 *
	 * If stored value is not a list, it is replaced with a list that has all
	 * indexes up to @p ix filled with invalid SValue.
	 *
	 * If the @p val is not a SValue, it is converted to it. If it is list or map,
	 * conversion is done recursively.
	 *
	 * @param ix Index of item to be set
	 * @param val New value
	 */
	void setProperty(int ix, const QVariant &val);

	/**
	 * Treats the stored value as a list and sets value at index @p ix to value @p val
	 *
	 * If stored value is not a list, it is replaced with a list that has all
	 * indexes up to @p ix filled with invalid SValue.
	 *
	 * @param ix Index of item to be set
	 * @param val New value
	 */
	void setProperty(int ix, const SValue &val) {QVariant v; v.setValue(val); setProperty(ix, v);}

	void setPropertyOnPath(const QStringList &path, const QVariant &val) const;
	void setPropertyOnPath(const QString &path, const QVariant &val) const {return setPropertyOnPath(path.split('.'), val);}

	/// Do hodnoty vracene timto operatorem lze diky explicitnimu sdileni priradit.
	/// Pokud navraceny objekt referencuje nejake misto ve stromu, je treba mit na pozoru, co se pri prirazeni skutecne deje.
	/// Strom je modifikovan pouze v pripade, ze je prirazen QVariant jineho typu nez SValue
	/// pokud se prirazuje neco jineho nez SValue operator= se chova jako setValue()
	/// pokud se prirazuje SValue operator= se chova jako pretoceni dat na data svalue na prave strane prirazeni
	/**
		\code
		SValue sv;
		sv["rows"][10] = 1; /// dojde k modifikaci sv

		SValue sv3;
		QVariantList vl;
		vl << 1 << 2 << 3;
		sv3["rows"] = vl;  /// dojde k modifikaci sv

		sv["rows"][10] = sv3 /// strom zustane nezmenen, protoze sv["rows"][10] vrati SValue, ktera referencuje sv["rows"][10], ale pri prirazeni se tato reference pretoci na data sv3
		sv["rows"].setProperty(10, sv3) nebo sv["rows"][10].setValue(sv3)  /// toto je OK

		SValue sv4 = sv["rows"][10]; // vznikne promenna sv4, ktera referencuje svalue obsahujici hodnotu 1, ta rovnez nahradi primitivni hodnotu 1, ktera byla v listu dosud. viz. property_helper()
		// z nasl. radku odkomentujte pouze 1
		//sv["rows"][10] = sv3; // nestane se nic, vznikne svalue obsahujici hodnotu 1, ktera nahradi primitivni hodnotu 1, ktera byla v listu dosud. viz. property_helper()
		sv["rows"].setProperty(10, sv3); // sv se zmeni, sv["rows"][10] nyni referencuje stejna data, jako sv3
		//sv4 = sv3; // nestane se nic, sv4 ted proste referencuje data, ktera refrencuje sv3
		//sv4 = sv3.value(); // dojde k prirazeni do sv["rows"][10], sv se zmeni
		//sv4 = 3; // dojde k prirazeni do sv["rows"][10], sv se zmeni
		\endcode
		\code
		SValue sv;
		SValue sv3;
		QVariantList vl;
		vl << 1 << 2 << 3;
		// Bacha na to
		sv[10] = sv3; // nestane se nic, viz. priklady nahore. sv[10] vraci svalue referencujici 10ty prvek listu, prirazenim se stane pouze to, ze vracena svalue bude referencovat stejna data jako sv3
		sv[10] = 3; // to je OK, dojde k prirazeni 3 to sv[10] (k prirazeni dojde vzdy pokud na prave strane neni QFScriptValue)
		sv.setProperty(10, sv3); // toto je treba pouzit pro prirazeni SValue do sv[10]
		\endcode
		*/
	SValue operator[](const QString &name) {return property_helper(name);}
	SValue operator[](int ix) {return property_helper(ix);}

	/**
	 * Assigns shallow copy of given variant to the data referenced by this object
	 *
	 * @param v Variant to be assigned
	 * @return reference to self
	 */
	SValue& operator=(const QVariant &v) {setValue(v); return *this;}
	///SValue& operator=(const SValue &sv); tento operator je implicitni, neni treba ho psat
	SValue& operator+=(const QVariantMap &m);
	bool operator==(const SValue &other) const {return d == other.d;}
	bool operator!=(const SValue &other) const {return d != other.d;}
public:
	/// convert SValue to nested variants maps
	QVariant toVariant() const;
	/// wrap SValue to QVariant
	QVariant variantCast() const;

	/// rozparsuje JSON a vyrobi rekurzivne cely strom
	void setJson(const QByteArray &json);
	void setJson(const QVariant &json) {setVariant(json);}

	/**
	 * Converts given variant to SValue and assigns it to the data referenced by this object
	 * @param json Variant to be set
	 */
	void setVariant(const QVariant &json);

	// data can contain QJSValues from QML scripts, convert them to pure c++ QVariants (QVariantMap, QVariantList)
	//void removeJSTypes();

	static SValue fromJson(const QVariant &json) {SValue ret; ret.setJson(json); return ret;}

	/**
	 * Convert stored value to the JSON format
	 *
	 * @param format JSON format options
	 * @return Byte array with JSON document
	 */
	QByteArray toJson(QJsonDocument::JsonFormat format = QJsonDocument::Indented) const;
	QByteArray serialize() const;

	/**
	 * Convert stored value to the JSON format
	 *
	 * @param format JSON format options
	 * @return String with JSON document
	 */
	QString toString(QJsonDocument::JsonFormat format = QJsonDocument::Indented) const;
};

}}}

Q_DECLARE_METATYPE(qf::core::utils::SValue)

#endif

