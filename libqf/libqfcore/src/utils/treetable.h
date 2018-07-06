//
// Author: Frantisek Vacek <fanda.vacek@volny.cz>, (C) 2014
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef QF_CORE_UTILS_TREETABLE_H
#define QF_CORE_UTILS_TREETABLE_H

#include "../core/coreglobal.h"
#include "svalue.h"

#include <QVariantMap>
#include <QVariantList>

namespace qf {
namespace core {
namespace utils {

/// helper table used for repor generator mainly
/// should be replaced by Table in future
class QFCORE_DECL_EXPORT TreeTableColumn : public SValue
{
public:
	QString name() const;
	void setName(const QString &n);
	QString typeName() const;
	void setTypeName(const QString &tn);
	QVariant::Type type() const;
	void setType(QVariant::Type t);
	QString width() const;
	void setWidth(const QString &w);
	QString header() const;
	QString footer() const;
	QString hAlignment() const;
public:
	TreeTableColumn() : SValue() {}
	TreeTableColumn(const SValue &sv) : SValue(sv) {}
	//virtual ~TreeTableRow();
};


class QFCORE_DECL_EXPORT TreeTableColumns : public SValue
{
protected:
public:
	int indexOf(const QString &col_name) const;
	TreeTableColumn column(int ix) const {return SValue(property(ix));}
	TreeTableColumn column(const QString &col_name) const {return column(indexOf(col_name));}
	//SValue column(const QString &col_name) const {return value(indexOf(col_name)).toMap();}
public:
	TreeTableColumns() : SValue() {}
	TreeTableColumns(const QVariant &v);
	//virtual ~TreeTableRow();
};


class QFCORE_DECL_EXPORT TreeTableRow// : public SValue
{
protected:
	TreeTableColumns f_columns;
	SValue f_row;
protected:
	void convertListToMap();
	QVariant value_helper(int ix, bool &found) const;
	QVariant value_helper(const QString &col_or_key_name, bool &found) const;
public:
	bool isNull() const {return !f_row.isValid();}
	TreeTableColumns columns() const {return f_columns;}

	SValue rowData() const {return f_row;}

	QVariant value(int col) const;
	QVariant value(int col, const QVariant &default_val) const;
	QVariant value(const QString &col_or_key_name) const;
	QVariant value(const QString &col_or_key_name, const QVariant &default_val) const;
	void setValue(int col, const QVariant &val);
	void setValue(const QString &col_or_key_name, const QVariant &val);

	SValue keyvals() const;
	SValue keyvalsRef();

	int tablesCount() const;
	SValue table(int ix) const;
	SValue table(const QString &name = QString()) const;
	void appendTable(const SValue &t);
public:
	TreeTableRow() {}
	TreeTableRow(const TreeTableColumns &columns, const SValue &row_data);
	virtual ~TreeTableRow();
};

/**
 * Extension of SValue that treats the referenced data as a table.
 *
 * It uses map with following keys:
 * * type: QVariant enum of stored data type
 * * meta: some metadata of the tale. Currently only "name" is used
 * * fields: info about columns. @see TreeTableColumn
 * * rows: list of rows of the table. Each row can be either a list of column values,
 *     or a map, where key "row" is list of column values and key "tables" is list of
 *     additional tables. @see TreeTableRow
 * * keyvals: map of additional user data
 */
class QFCORE_DECL_EXPORT TreeTable : public SValue
{
public:
	static const QString KEY_TYPE;
	static const QString KEY_META;
	static const QString KEY_NAME;
	static const QString KEY_COLUMNS;
	static const QString KEY_ROWS;
	static const QString KEY_ROW;
	static const QString KEY_KEYVALS;
	static const QString KEY_TABLES;
public:
	/**
	 * Creates empty table
	 */
	TreeTable();

	/**
	 * Creates empty table with name set
	 *
	 * @param table_name Name of the table
	 */
	TreeTable(const QString &table_name) {setTableName(table_name);}

	/**
	 * Creates table referencing given SValue.
	 *
	 * @param sv SValue to be referenced
	 */
	TreeTable(const SValue &sv) : SValue(sv) {}

	/**
	 * Just decreases the reference counter
	 */
	virtual ~TreeTable();
protected:
	/**
	 * Gets the rows
	 *
	 * @return SValue referencing the rows or invalid SValue if rows are not available
	 */
	SValue rows() const {return SValue(property(KEY_ROWS));}
	//void setRows(const QVariantList &new_rows) {f_data[KEY_ROWS] = new_rows;}
	//void setColumns(const TreeTableColumns &cols) {f_data[KEY_COLUMNS] = cols;}
public:
	bool isNull() const {return !isValid();}
	QString name() const {return property2(KEY_META).property2(KEY_NAME).value().toString();}
	void setName(const QString &n) {(*this)[KEY_META][KEY_NAME] = n;}
	/**
	 * Get definitions of the columns
	 */
	TreeTableColumns columns() const {return TreeTableColumns(property(KEY_COLUMNS));}

	/**
	 * Get number of columns
	 */
	int columnCount() const {return columns().count();}

	/**
	 * Get number of rows
	 */
	int rowCount() const;

	/**
	 * Get the row
	 *
	 * @param ix Index of the row to be returned
	 */
	TreeTableRow row(int ix) const {return TreeTableRow(columns(), rows().property(ix));}

	/**
	 * Appends new row
	 *
	 * @return Reference to appended row
	 */
	TreeTableRow appendRow();
	void removeRow(int ix);

	void setColumnHeader(const QString &col_name, const QString &header);

	/**
	 * Get header of a column
	 *
	 * @param col_name Name of the column
	 * @return Column header or the @p col_name, if column is not present
	 */
	QString columnHeader(const QString &col_name) const;
	void setColumnFooter(const QString &col_name, const QString &footer);
	/**
	 * Get footer of a column
	 *
	 * @param col_name Name of the column
	 * @return Column header or empty string, if column is not present
	 */
	QString columnFooter(const QString &col_name) const;
	/// pokud je tabulka v reportu generovana z dat, je sloupec takto zarovnan
	void setColumnAlignment(const QString &col_name, Qt::Alignment alignment);
	void setColumnAlignment(int col_no, Qt::Alignment alignment);
	//QString columnHAlignment(const QString &col_name) const;

	//QString columnWidth(int col_no) const;

	/**
	 * Adds new column
	 *
	 * @param name Name of the column
	 * @param type Optional type of values in the column. If not given, string is expected
	 * @param caption Optional header of the column. If not given, header is not set
	 * @return Reference to created column.
	 */
	TreeTableColumn appendColumn(const QString &name, QVariant::Type type = QVariant::String, const QString &caption = QString());

	//bool isNull() const {return columnCount() == 0;}

	//! @param key_ends_with if true key name is compared using function QFSql::endsWith().
	/// pokud se vyskytuje agregacni funkce, musi byt okolo jmena fieldu, napr. SUM(cena)

	/**
	 * Gets various values from the table.(But not the values stored in the table)
	 *
	 * The value to be returned is specified by the @p key_name column.
	 * It can start with a path to table in format (<row_idx>/<table_name>/)*.
	 * It then returns the given key_name from table at given path. @see cd
	 *
	 * @p key_name can also be one of these functions:
	 * * SUM(<column>): it returns sum of values in given column
	 * * AVG(<column>): it returns average of values in given column
	 * * CNT(): it returns number of rows in the table
	 * * CAPTION(<column>): returns header of the column
	 * * HEADER(<column>): returns header of the column
	 * * FOOTER(<column>): gets footer of given column
	 *
	 * If the @p key_name is not a function, matching value is searched in the keyvals.
	 * If the @p key_ends_with is true, match is recognized even if only end of
	 * the keyval equals to the key_name or end of the key_name equals to the keyval.
	 *
	 * @param key_name Specification of value to be returned
	 * @param default_val value returned, if @p key_name wasn't found in the table.
	 * @param key_ends_with
	 */
	QVariant value(const QString &key_name, const QVariant &default_val = QVariant(), bool key_ends_with = true) const;

	/**
	 * Sets given keyval to given value.
	 * @param key_name Name of the keyval.
	 * @param val Value to be set.
	 */
	void setValue(const QString &key_name, const QVariant &val);

	/**
	 * Computes sum of values in given column.
	 *
	 * @param col_name Name of the column.
	 * @return Integer or double sum of the column, or invalid variant, if column is not present.
	 */
	QVariant sum(const QString &col_name) const;

	/**
	 * Computes sum of values in given column.
	 *
	 * @param col_index Index of the column.
	 * @return Integer or double sum of the column, or invalid variant, if column is not present.
	 */
	QVariant sum(int col_index) const;

	/**
	 * Computes average of values in given column.
	 *
	 * @param col_name Name of the column.
	 * @return Double value or invalid variant, if column is not present or no rows in the table.
	 */
	QVariant average(const QString &col_name) const;

	/**
	 * Computes average of values in given column.
	 *
	 * @param col_index Index of the column.
	 * @return Double value or invalid variant, if column is not present or no rows in the table.
	 */
	QVariant average(int col_index) const;

	QString tableName() const;
	void setTableName(const QString &name);

	/**
	 * Get the keyvals
	 */
	SValue keyvals();
	const SValue keyvals() const;

	/// hleda tabulku v detech
	/// cesta ma format n/table_name1/n/table_name2/... , kde n je cislo radku, ve kterem se ma tabulka hledat
	TreeTable cd(const QString &path) const;

	/// options:
	/// lineSeparator: cim se budou oddelovat radky, default je '\n'
	/// lineIndent: cim se budou uvozovat vnorene urovne radku, default jsou dve mezery
	/// lineOffset: cim se budou uvozovat vschny radky, default je QString()
	QString toHtml(const QVariantMap &opts = QVariantMap()) const;
};

}}}

Q_DECLARE_METATYPE(qf::core::utils::TreeTable)

#endif // TreeTable_H

