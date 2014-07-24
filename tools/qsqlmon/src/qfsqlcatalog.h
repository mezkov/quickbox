#ifndef QFSQLCATALOG_H
#define QFSQLCATALOG_H

#include <QSqlField>

class QSqlDatabase;

class SqlUtils
{
public:
	SqlUtils(const QSqlDatabase &connection)
	{
		m_connection = connection;
	}

public:
	void setCurrentSchema(const QString &schema_name);

	//! retrieves CREATE TABLE ... Sql script for \a tblname.
	QString createTableSqlCommand(const QString &tblname);

	//! retrieves INSERT INTO ... Sql script for \a tblname.
	QString dumpTableSqlCommand(const QString &tblname);

	//! take CREATE TABLE ... and parse fields definitions from it.
	static QStringList fieldDefsFromCreateTableCommand(const QString &cmd);
private:
	QSqlDatabase m_connection;
};

template <class T>
class QFSqlInfoList : public QMap<QString, T>
{
public:
	QStringList unorderedKeys() const {return m_unorderedKeys;}
	T& addEntry(const QString &key)
	{
		m_unorderedKeys << key;
		return this->operator[](key);
	}

	virtual void load(const QSqlDatabase &connection, const QString parent_object_id)
	{
		Q_UNUSED(connection);
		m_parentObjectId = parent_object_id;
	}

protected:
	QStringList m_unorderedKeys;
	QString m_parentObjectId;
};

class QFSqlFieldInfo : public QSqlField
{
private:
	struct Data
	{
		QString fulltablename;
		QString seqname;
		quint8 nullable:1, isunsigned:1, autoincrement:1;
		QString characterSet;
		QString collation;
		QString comment;
		QVariantMap nativeValues;
		Data(const QString& full_tbl_name)
			: fulltablename(full_tbl_name), nullable(true), isunsigned(false), autoincrement(false) {}
	};
	Data data;
public:
	//! \a fullName always starts with '.'
	QString fullName() const {return data.fulltablename + '.' + name();}
	QString shortName() const {return name();}
	//bool isValid() const {return QSqlField::isValid();}
	QString seqName() const {return data.seqname;} //!< name of sequence, if field is autogenerated
	void setSeqName(const QString &n) {data.seqname = n;}
	const QVariantMap& nativeValues() const {return data.nativeValues;}
	QVariant nativeValue(const QString &key) const {return nativeValues().value(key);}
	QVariantMap& nativeValuesRef() {return data.nativeValues;}
	QStringList enumOrSetFields() const {return nativeValue("enumOrSetFields").toStringList();}
	QString nativeType() const {return data.nativeValues.value("nativeType").toString();}
	void setNativeType(const QString &s) {data.nativeValues["nativeType"] = s;}
	QString characterSet() const {return data.characterSet;}
	void setCharacterSet(const QString &s) {data.characterSet = s;}
	QString collation() const {return data.collation;}
	void setCollation(const QString &s) {data.collation = s;}
	QString comment() const {return data.comment;}
	void setComment(const QString &s) {data.comment = s;}
	bool isNullable() const {return data.nullable;}
	void setNullable(bool b) {data.nullable = b;}
	bool isUnsigned() const {return data.isunsigned;}
	void setUnsigned(bool b) {data.isunsigned = b;}
	bool isAutoIncrement() const {return data.autoincrement;}
	void setAutoIncrement(bool b) {data.autoincrement = b;}
	QVariant seqNextVal();
	QString toString(const QString &indent = QString()) const;
public:
	QFSqlFieldInfo(const QSqlField& fld = QSqlField(), const QString& full_tbl_name = QString())
		: QSqlField(fld), data(full_tbl_name)
	{
	}
	QFSqlFieldInfo& operator=(const QSqlField &other)
	{
		(*static_cast<QSqlField*>(this)) = other;
		return *this;
	}
};

class QFSqlFieldInfoList : public QFSqlInfoList<QFSqlFieldInfo>
{
private:
	typedef QFSqlInfoList<QFSqlFieldInfo> Super;
public:
	void load(const QSqlDatabase &connection, const QString table_id) Q_DECL_OVERRIDE;
};


struct QFSqlIndexInfo
{
	QString name;
	bool unique;
	bool primary;
	QStringList fields;

	QFSqlIndexInfo() : unique(false), primary(false) {}

	static QList<QFSqlIndexInfo> loadIndexes(const QSqlDatabase &connection, const QString table_id);
};

#endif // QFSQLCATALOG_H