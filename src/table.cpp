#include "table.h"
#include <QDebug>

Column::Column(QString name, QString type) {
	_name = name;
	_type = type;
}

Table::Table(QString name) {
	_name = name;
}

Table::~Table() {
	while(!_columns.isEmpty()) {
		delete _columns.takeLast();
	}
}

void Table::addColumn(Column* const column) {
	_columns.append(column);
}

int Table::getColumnId(const QString& name) {
	for(int i = 0; i < _columns.count(); ++i) {
		if(_columns[i]->getName() == name) {
			return i;
		}
	}
	return -1;
}

QString Table::getColumnName(const int id) {
	return _columns[id]->getName();
}

QString Table::definitionForCreate() {
	QString retVal;
	for(int i = 0; i < _columns.count(); ++i) {
		retVal.append(_columns[i]->getName() + " " + _columns[i]->getType() + ", ");
	}
	retVal.chop(2);
	return retVal;
}

QString Table::definitionForInsert() {
	QString retVal;
	for(int i = 1; i < _columns.count(); ++i) {
		retVal.append(_columns[i]->getName() + ", ");
	}
	retVal.chop(2);
	return retVal;
}
