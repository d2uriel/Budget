#ifndef TABLE_H
#define TABLE_H

#include <QList>
#include <QString>

class Column {
public:
	// Methods.
	Column(QString name, QString type);

	QString getName() { return _name; }
	void setName(const QString& name) { _name = name; }

	QString getType() { return _type; }
	void setType(const QString& type) { _type = type; }

private:
	// Members.
	QString _name;
	QString _type;
};

class Table
{
public:
	// Methods.
	Table(QString name);
	~Table();

	int			getColumnCount() { return _columns.count(); }
	QString		getName() { return _name; }

	void		addColumn(Column* const column);
	int			getColumnId(const QString& name);
	QString		getColumnName(const int id);
	QString		definitionForCreate();
	QString		definitionForInsert();

private:
	// Members.
	QString _name;
	QList<Column*> _columns;

};

#endif // TABLE_H
