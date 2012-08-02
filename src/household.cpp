#include "household.h"
#include "loginDialog.h"
#include <QMessageBox>
#include <QFile>
#include <QDateTime>
#include <QDebug>
#include <QtAlgorithms>
#include <QCryptographicHash>

Household::Household() {
	qDebug() << "Household::Household()";
	_name = "";
	_path = "";
	_db = 0;
	_categories = new QList<Category*>;
	_dataList = new QList<Data*>;
	_tables = new QList<Table*>;
	_monthStartsAt = MONTH_STARTS_LAST_INCOME;
	_lastSortType = 0;
	_columnVisibility = DT_ALL;
	setupTables();
}

Household::~Household() {
	qDebug() << "Household::~Household()";
	if(_db != 0) {
		if(_db->isOpen()) {
			_db->close();
		}
		delete _db;
	}
	QSqlDatabase::removeDatabase(_name);
	while(!_tables->isEmpty()) {
		delete _tables->takeLast();
	}
}

Table* Household::getTable(const QString& name) {
	//qDebug() << "Household::getTable()";
	for(int i = 0; i < _tables->count(); ++i) {
		if(_tables->at(i)->getName() == name) {
			return _tables->at(i);
		}
	}
	return 0;
}

void Household::setupTables() {
	qDebug() << "Household::setupTables()";
	Table *table = new Table(TABLE_HOUSEHOLD);
	table->addColumn(new Column(COLUMN_ID.name, COLUMN_ID.type));
	table->addColumn(new Column(COLUMN_NAME.name, COLUMN_NAME.type));
	table->addColumn(new Column(COLUMN_DATETIME.name, COLUMN_DATETIME.type));
	_tables->append(table);

	table = new Table(TABLE_CATEGORIES);
	table->addColumn(new Column(COLUMN_ID.name, COLUMN_ID.type));
	table->addColumn(new Column(COLUMN_PID.name, COLUMN_PID.type));
	table->addColumn(new Column(COLUMN_NAME.name, COLUMN_NAME.type));
	table->addColumn(new Column(COLUMN_TYPE.name, COLUMN_TYPE.type));
	table->addColumn(new Column(COLUMN_USERID.name, COLUMN_USERID.type));
	_tables->append(table);

	table = new Table(TABLE_DATA);
	table->addColumn(new Column(COLUMN_ID.name, COLUMN_ID.type));
	table->addColumn(new Column(COLUMN_DATA_TYPE.name, COLUMN_DATA_TYPE.type));
	table->addColumn(new Column(COLUMN_DATETIME.name, COLUMN_DATETIME.type));
	table->addColumn(new Column(COLUMN_QTY.name, COLUMN_QTY.type));
	table->addColumn(new Column(COLUMN_VALUE.name, COLUMN_VALUE.type));
	table->addColumn(new Column(COLUMN_CATEGORY_ID.name, COLUMN_CATEGORY_ID.type));
	table->addColumn(new Column(COLUMN_SUM.name, COLUMN_SUM.type));
	table->addColumn(new Column(COLUMN_USERID.name, COLUMN_USERID.type));
	table->addColumn(new Column(COLUMN_ACCOUNT_ID.name, COLUMN_ACCOUNT_ID.type));
	table->addColumn(new Column(COLUMN_DETAILS.name, COLUMN_DETAILS.type));
	table->addColumn(new Column(COLUMN_MONTH_BEGIN.name, COLUMN_MONTH_BEGIN.type));
	_tables->append(table);

	table = new Table(TABLE_ACCOUNTS);
	table->addColumn(new Column(COLUMN_ID.name, COLUMN_ID.type));
	table->addColumn(new Column(COLUMN_NAME.name, COLUMN_NAME.type));
	table->addColumn(new Column(COLUMN_VALUE.name, COLUMN_VALUE.type));
	table->addColumn(new Column(COLUMN_DETAILS.name, COLUMN_DETAILS.type));
	table->addColumn(new Column(COLUMN_USERID.name, COLUMN_USERID.type));
	table->addColumn(new Column(COLUMN_DEFAULT.name, COLUMN_DEFAULT.type));
	_tables->append(table);

	table = new Table(TABLE_CONFIG);
	table->addColumn(new Column(COLUMN_TYPE.name, COLUMN_TYPE.type));
	table->addColumn(new Column(COLUMN_NAME.name, COLUMN_NAME.type));
	_tables->append(table);

	table = new Table(TABLE_USERS);
	table->addColumn(new Column(COLUMN_ID.name, COLUMN_ID.type));
	table->addColumn(new Column(COLUMN_NAME.name, COLUMN_NAME.type));
	table->addColumn(new Column(COLUMN_PASSWORD.name, COLUMN_PASSWORD.type));
	table->addColumn(new Column(COLUMN_DETAILS.name, COLUMN_DETAILS.type));
	table->addColumn(new Column(COLUMN_ACCESS_LEVEL.name, COLUMN_ACCESS_LEVEL.type));
	_tables->append(table);

	table = new Table(TABLE_TRANSFERS);
	table->addColumn(new Column(COLUMN_ID.name, COLUMN_ID.type));
	table->addColumn(new Column(COLUMN_FROM_ID.name, COLUMN_FROM_ID.type));
	table->addColumn(new Column(COLUMN_TO_ID.name, COLUMN_TO_ID.type));
	table->addColumn(new Column(COLUMN_VALUE.name, COLUMN_VALUE.type));
	table->addColumn(new Column(COLUMN_USERID.name, COLUMN_USERID.type));
	_tables->append(table);
}

QString Household::buildInsertQueryStr(Table* const table, const QString& values) {
	qDebug() << "Household::buildInsertQueryStr()";
	QList<QString> valuesList = values.split(COMMA);
	QString retVal;
	retVal.append("INSERT INTO " + table->getName() + "(" + table->definitionForInsert() + ") VALUES (" + APO);
	for(int i = 0; i < valuesList.count(); ++i) {
		retVal.append(valuesList[i] + APOCOMMAAPO);
	}
	retVal.chop(3);
	retVal.append(")");
	return retVal;
}

QString Household::buildUpdateQueryStr(Table* const table, const QString& values, const qlonglong id) {
	qDebug() << "Household::buildUpdateQueryStr()";
	QList<QString> valuesList = values.split(COMMA);
	QString retVal;
	retVal.append("UPDATE " + table->getName() + " SET ");
	for(int i = 0; i < valuesList.count(); ++i) {
		retVal.append(table->getColumnName(i + 1) + EQU + APO + valuesList[i] + APO + COMMA);
	}
	retVal.chop(2);
	retVal.append(" WHERE " + COLUMN_ID.name + EQU + APO + QString::number(id) + APO);
	return retVal;
}

QString Household::buildDeleteQueryStr(Table *const table, const COLUMN& column, const qlonglong id) {
	qDebug() << "Household::buildDeleteQueryStr()";
	QString retVal = "DELETE FROM " + table->getName() + " WHERE " + column.name + EQU + APO + QString::number(id) + APO;
	return retVal;
}

bool Household::executeQuery(const QString& queryStr) {
	qDebug() << "Household::executeQuery()";
	QSqlQuery query(*_db);
	if(!query.exec(queryStr)) {
		qDebug() << "Failed to execute query:" << queryStr << ", reason:" << query.lastError().text();
		return false;
	} else {
		return true;
	}
}

bool Household::executeQueries(QList<QString> queries) {
	qDebug() << "Household::executeQueries()";
	if(_db->transaction()) {
		while(!queries.isEmpty()) {
			executeQuery(queries.takeFirst());
		}
		if(!_db->commit()) {
			qDebug() << "Failed executing transaction. Reason:" << _db->lastError().text();
			if(!_db->rollback()) {
				qDebug() << "Failed rolling back the transaction. Reason:" << _db->lastError().text();
			}
			return false;
		}
	} else {
		return false;
	}
	return true;
}

qlonglong Household::qtyIntFromMoneyString(QString s) {
	qDebug() << "Household::qtyIntFromMoneyString()";
	QList<QString> parts = s.split(",");
	if(parts.count() == 1) {
		parts = s.split(".");
	}
	qlonglong value = 0;
	value += parts[0].toLongLong() * 1000;
	if(parts.count() == 2) {
		if(parts[1].count() == 1) {
			value += parts[1].toLongLong() * 100;
		} else if(parts[1].count() == 2) {
			value += parts[1].toLongLong() * 10;
		} else if(parts[1].count() == 3) {
			value += parts[1].toLongLong();
		}
	}
	return value;
}

qlonglong Household::valueIntFromMoneyString(QString s) {
	qDebug() << "Household::valueIntFromMoneyString()";
	QList<QString> parts = s.split(",");
	if(parts.count() == 1) {
		parts = s.split(".");
	}
	qlonglong value = 0;
	value += parts[0].toLongLong() * 100;
	if(parts.count() == 2) {
		if(parts[1].count() == 1) {
			value += parts[1].toLongLong() * 10;
		} else if(parts[1].count() == 2) {
			value += parts[1].toLongLong();
		}
	}
	return value;
}

QString Household::stringFromValueInt(qlonglong i) {
	qDebug() << "Household::stringFromValueInt()";
	QString s = QString::number(i);
	if(s.count() == 1) {
		return QString("0,0" + s);
	} else if (s.count() == 2) {
		return QString("0," + s);
	} else if (s.count() > 2) {
		QString zl = s.left(s.count() - 2);
		QString gr = s.right(2);
		return QString(zl + "," + gr);
	} else {
		return QString("");
	}
}

QString Household::stringFromQtyInt(qlonglong i) {
	qDebug() << "Household::stringFromQtyInt()";
	QString s = QString::number(i);
	if(s.count() == 1) {
		return QString("0,00" + s);
	} else if(s.count() == 2) {
		return QString("0,0" + s);
	} else if(s.count() == 3) {
		return QString("0," + s);
	} else if (s.count() > 3) {
		QString zl = s.left(s.count() - 3);
		QString gr = s.right(3);
		return QString(zl + "," + gr);
	} else {
		return QString("");
	}
}

qlonglong Household::multiplyToRoundedInt(const qlonglong qty, const qlonglong value) {
	qDebug() << "Household::multiplyToRoundedInt()";
	qlonglong retVal = 0;
	retVal = qty * value;
	if(retVal % 1000 >= 500) {
		retVal += 1000;
	}
	retVal -= (retVal % 1000);
	return retVal / 1000;
}

bool Household::openDB() {
	qDebug() << "Household::openDB()";
	_db = new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE", _name));
	_db->setDatabaseName(_path + "/" + _name + DB_EXT);
	if(!_db->open()) {
		qDebug() << "Failed to open DB";
		return false;
	}
	return true;
}

bool Household::loadUsers() {
	qDebug() << "Household::loadUsers()";
	while(!_users.isEmpty()) {
		delete _users.takeLast();
	}
	QSqlQuery query(*_db);
	QString queryStr = "SELECT * FROM " + TABLE_USERS;
	if(query.exec(queryStr)) {
		while(query.next()) {
			User* u = new User;
			u->id = query.value(getTable(TABLE_USERS)->getColumnId(COLUMN_ID.name)).toLongLong();
			u->name = query.value(getTable(TABLE_USERS)->getColumnId(COLUMN_NAME.name)).toString();
			u->description = query.value(getTable(TABLE_USERS)->getColumnId(COLUMN_DETAILS.name)).toString();
			u->password = query.value(getTable(TABLE_USERS)->getColumnId(COLUMN_PASSWORD.name)).toString();
			u->level = query.value(getTable(TABLE_USERS)->getColumnId(COLUMN_ACCESS_LEVEL.name)).toInt();
			_users.append(u);
		}
	} else {
		QMessageBox::warning(this, tr("Error"), tr("Failed to load user list."));
		return false;
	}
	return true;
}

bool Household::loadDB() {
	qDebug() << "Household::loadDB()";
	// Check DB version.
	QSqlQuery query(*_db);
	QString queryStr = "SELECT " + COLUMN_DATETIME.name + " FROM " + TABLE_HOUSEHOLD + " WHERE " + COLUMN_NAME.name + EQU \
			+ APO + VERSION_STR + APO;
	if(query.exec(queryStr)) {
		if(query.next()) {
			_dbVersion = query.value(0).toLongLong();
			if(_dbVersion < DB_VERSION) {
				if(QMessageBox::question(this, tr("Question"), tr("This is an old household database version.\nDo you want to update it?"), \
										 QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes) {
					updateDB();
				} else {
					QMessageBox::information(this, tr("Error"), tr("Unable to open household.\nHousehold DB version older than required."));
					return false;
				}
			} else if(_dbVersion > DB_VERSION){
				QMessageBox::information(this, tr("Error"), tr("Unable to open household.\nHousehold DB version newer than required.\nPlease update this application."));
				return false;
			} else {

			}
		}
	} else {
		QMessageBox::warning(this, tr("Error"), tr("Failed to get DB version."));
		return false;
	}
	bool success = loadUsers();
	if(success) {
		success = loadAccounts();
		if(success) {
			success = loadCategories();
			if(success) {
				if(_monthStartsAt == MONTH_STARTS_LAST_INCOME) {
					success = loadDataList(getLastIncomeDate(), QDateTime::currentDateTime());
				} else if(_monthStartsAt == MONTH_STARTS_FIRST_DAY) {
					QDate currentDate = QDateTime::currentDateTime().date();
					success = loadDataList(QDateTime(QDate(currentDate.year(), currentDate.month(), 1)), QDateTime::currentDateTime());
				}
			}
		}
	}
	return success;
}

bool Household::updateDB() {
	qDebug() << "Household::updateDB()";
	return true;
}

bool Household::createDB() {
	qDebug() << "Household::createDB()";
	bool error = false;
	if(_name.isEmpty() || _path.isEmpty()) {
		error = true;
	}
	if(openDB() && !error) {
		if(!createTables()) {
			error = true;
		}
	} else {
		error = true;
	}
	if(error) {
		// Revert everything.
		if(_db != 0) {
			if(_db->isOpen()) {
				_db->close();
			}
			delete _db;
			_db = 0;
			QSqlDatabase::removeDatabase(_name);
			if(QFile::exists(_path + "/" + _name + DB_EXT)) {
				QFile DBfile(_path + "/" + _name + DB_EXT);
				DBfile.remove();
			}
		}
		qDebug() << "Failed to create DB.";
		return false;
	} else {
		return true;
	}

}

bool Household::createTable(Table* const table) {
	qDebug() << "Household::createTable()";
	QString queryStr;
	queryStr = "CREATE TABLE " + table->getName() + "(" + table->definitionForCreate() + ")";
	if(!executeQuery(queryStr)) {
		qDebug() << "Failed to create table.";
		return false;
	}
	return true;
}

bool Household::createTables() {
	qDebug() << "Household::createTables()";
	QString queryStr;

	if(createTable(getTable(TABLE_HOUSEHOLD))) {
		// Fill table with data.
		queryStr = buildInsertQueryStr(getTable(TABLE_HOUSEHOLD), _name + COMMA + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
		if(!executeQuery(queryStr)) {
			qDebug() << "Failed to insert data to table.";
			return false;
		} else {
			queryStr = buildInsertQueryStr(getTable(TABLE_HOUSEHOLD), VERSION_STR + COMMA + QString::number(DB_VERSION));
			if(!executeQuery(queryStr)) {
				qDebug() << "Failed to insert data to table.";
				return false;
			}
		}
	} else {
		return false;
	}

	if(!createTable(getTable(TABLE_CATEGORIES))) {
		return false;
	}

	if(!createTable(getTable(TABLE_DATA))) {
		return false;
	}

	if(!createTable(getTable(TABLE_CONFIG))) {
		return false;
	}

	if(!createTable(getTable(TABLE_ACCOUNTS))) {
		return false;
	}

	if(!createTable(getTable(TABLE_USERS))) {
		return false;
	}

	if(!createTable(getTable(TABLE_TRANSFERS))) {
		return false;
	}
	return true;
}

const QList<Category*> Household::getParentCategories() {
	qDebug() << "Household::getParentCategories()";
	QList<Category*> parentCategories;
	for(int i = 0; i < _categories->count(); ++i) {
		Category* c = _categories->at(i);
		if(c->pid == -1) {
			parentCategories.append(c);
		}
	}
	return parentCategories;
}

const QList<QString> Household::getParentCategoryNames(const int type) {
	qDebug() << "Household::getParentCategories()";
	QList<QString> parentCategoryNames;
	for(int i = 0; i < _categories->count(); ++i) {
		Category* c = _categories->at(i);
		if(c->type == type && c->pid == -1) {
			parentCategoryNames.append(c->name);
		}
	}
	return parentCategoryNames;
}

const QList<Category*> Household::getChildCategories() {
	qDebug() << "Household::getChildCategories()";
	QList<Category*> childCategories;
	for(int i = 0; i < _categories->count(); ++i) {
		Category* c = _categories->at(i);
		if(c->pid != -1) {
			childCategories.append(c);
		}
	}
	return childCategories;
}

const QList<Category*>* Household::getCategories() {
	qDebug() << "Household::getCategories()";
	return _categories;
}

bool Household::addCategory(const Category& c, const bool refresh) {
	qDebug() << "Household::addCategory()";
	QSqlQuery query(*_db);
	QString queryStr = "SELECT " + COLUMN_NAME.name + " FROM " + TABLE_CATEGORIES + " WHERE " \
			+ COLUMN_NAME.name + EQU + APO + c.name + APO;
	// Check if category already exists.
	if(query.exec(queryStr)) {
		if(query.first()) {
			QMessageBox::critical(this, tr("Error"), tr("Failed to add category. Category name already exists."));
			return false;
		}
	}

	queryStr = buildInsertQueryStr(getTable(TABLE_CATEGORIES), QString::number(c.pid) + COMMA + c.name + COMMA + QString::number(c.type) + COMMA + QString::number(currentUserId()));
	if(executeQuery(queryStr)) {
		if(refresh) {
			loadCategories();
		}
		return true;
	} else {
		qDebug() << "Failed to add category.";
		return false;
	}
}

bool Household::editCategory(const Category& c, const qlonglong id) {
	qDebug() << "Household::editCategory()";
	if(isCategoryInUse(id)) {
		QMessageBox::information(this, tr("Information"), tr("Unable to edit category.\nCategory is in use."));
		return false;
	}
	QString queryStr = buildUpdateQueryStr(getTable(TABLE_CATEGORIES), QString::number(c.pid) + COMMA + c.name + COMMA + QString::number(c.type) + COMMA + QString::number(currentUserId()), id);
	if(executeQuery(queryStr)) {
		loadCategories();
		return true;
	} else {
		qDebug() << "Failed to edit category.";
		return false;
	}
}

bool Household::removeCategory(const QString& name) {
	qDebug() << "Household::removeCategory()";
	QSqlQuery query(*_db);
	QString queryStr;
	// Check if category is in use.
	if(isCategoryInUse(getCategoryId(name))) {
		QMessageBox::information(this, tr("Information"), tr("Unable to remove category.\nCategory is in use."));
		return false;
	}

	// Remove child categories.
	queryStr = "SELECT " + COLUMN_ID.name + " FROM " + TABLE_CATEGORIES + " WHERE " + COLUMN_NAME.name + EQU + APO + name + APO;
	if(query.exec(queryStr)) {
		if(query.first()) {
			qlonglong pid = query.value(0).toLongLong();
			queryStr = buildDeleteQueryStr(getTable(TABLE_CATEGORIES), COLUMN_PID, pid); //"DELETE FROM " + TABLE_CATEGORIES + " WHERE " + COLUMN_PID.name + EQU + APO + QString::number(pid) + APO;
			if(!executeQuery(queryStr)) {
				qDebug() << "Failed to remove child categories.";
				return false;
			}
		}
	} else {
		qDebug() << "Failed to get child categories.";
		return false;
	}

	// Remove category.
	queryStr = buildDeleteQueryStr(getTable(TABLE_CATEGORIES), COLUMN_ID, getCategoryId(name)); //"DELETE FROM " + TABLE_CATEGORIES + " WHERE " + COLUMN_NAME.name + EQU + APO + name + APO;
	if(executeQuery(queryStr)) {
		loadCategories();
		return true;
	} else {
		qDebug() << "Failed to remove category.";
		return false;
	}
}

bool Household::loadCategories() {
	qDebug() << "Household::loadCategories()";
	while(!_categories->isEmpty()) {
		delete _categories->takeLast();
	}
	QSqlQuery query(*_db);
	QString queryStr = "SELECT * FROM " + TABLE_CATEGORIES;
	if(query.exec(queryStr)) {
		while(query.next()) {
			Category *c = new Category;
			c->id = query.value(getTable(TABLE_CATEGORIES)->getColumnId(COLUMN_ID.name)).toLongLong();
			c->pid = query.value(getTable(TABLE_CATEGORIES)->getColumnId(COLUMN_PID.name)).toLongLong();
			c->name = query.value(getTable(TABLE_CATEGORIES)->getColumnId(COLUMN_NAME.name)).toString();
			c->type = query.value(getTable(TABLE_CATEGORIES)->getColumnId(COLUMN_TYPE.name)).toInt();
			c->uid = query.value(getTable(TABLE_CATEGORIES)->getColumnId(COLUMN_USERID.name)).toLongLong();
			_categories->append(c);
		}
	} else {
		qDebug() << "Failed to load categories.";
		return false;
	}
	emit(categoriesChanged());
	return true;
}

bool Household::refreshCategories() {
	qDebug() << "Household::refreshCategories()";
	return loadCategories();
}

const QList<QString> Household::getCategoryNames(const int type) {
	qDebug() << "Household::getCategoryNames";
	QList<QString> categoryNames;
	for(int i = 0; i < _categories->count(); ++i) {
		if(_categories->at(i)->type == type || type == TYPE_ALL) {
			categoryNames.append(_categories->at(i)->name);
		}
	}
	return categoryNames;
}

qlonglong Household::getCategoryId(const QString& name) {
	qDebug() << "Household::getCategoryId()";
	for(int i = 0; i < _categories->count(); ++i) {
		if(_categories->at(i)->name == name) {
			return _categories->at(i)->id;
		}
	}
	return -1;
}

QString Household::getCategoryName(const qlonglong id) {
	qDebug() << "Household::getCategoryName()";
	if(id == -1) {
		return "";
	}
	for(int i = 0; i < _categories->count(); ++i) {
		if(_categories->at(i)->id == id) {
			return _categories->at(i)->name;
		}
	}
	return "";
}

int Household::getIncomeCategoryCount() {
	qDebug() << "Household::getIncomeCategoryCount()";
	int count = 0;
	for(int i = 0; i < _categories->count(); ++i) {
		if(_categories->at(i)->type == TYPE_INCOME) {
			++count;
		}
	}
	return count;
}

int Household::getExpensesCategoryCount() {
	qDebug() << "Household::getExpensesCategoryCount()";
	int count = 0;
	for(int i = 0; i < _categories->count(); ++i) {
		if(_categories->at(i)->type == TYPE_EXPENSE) {
			++count;
		}
	}
	return count;
}

qlonglong Household::addData(const Data& newData) {
	qDebug() << "Household::addData()";
	Table *table = getTable(TABLE_DATA);;
	qlonglong accountMoney = 0;
	if(newData.type == TYPE_INCOME) {
		accountMoney = getAccountMoney(newData.aid) + newData.sum;
	} else if(newData.type == TYPE_EXPENSE) {
		accountMoney = getAccountMoney(newData.aid) - newData.sum;
		if(accountMoney < 0) {
			QMessageBox::information(this, tr("Information"), tr("Not enough money on account to add expense."));
			return DATA_ADD_EDIT_NOT_SUCCESSFULL;
		}
	}
	QList<QString> queries;
	QString queryStr = buildInsertQueryStr(table, QString::number(newData.type) + COMMA + QString::number(newData.dateTime.toMSecsSinceEpoch()) + COMMA + QString::number(newData.qty) \
						+ COMMA + QString::number(newData.value) + COMMA + QString::number(newData.cid) + COMMA + QString::number(newData.sum) \
						+ COMMA + QString::number(currentUserId()) + COMMA + QString::number(newData.aid) + COMMA + newData.details + COMMA \
										   + (newData.isMonthStart ? "1" : "0"));

	queries.append(queryStr);

	queryStr = buildUpdateQueryStr(getTable(TABLE_ACCOUNTS), getAccountName(newData.aid) + COMMA + QString::number(accountMoney) + COMMA + getAccountDescription(newData.aid) + COMMA \
								   + QString::number(getAccountUserId(newData.aid)), newData.aid);
	queries.append(queryStr);

	if(!executeQueries(queries)) {
		qDebug() << "Failed to add data.";
		return DATA_ADD_EDIT_NOT_SUCCESSFULL;
	}
	loadAccounts();
	queryStr = "SELECT MAX(" + COLUMN_ID.name + ") FROM " + table->getName();
	QSqlQuery query(*_db);
	if(query.exec(queryStr)) {
		if(query.first()) {
			return query.value(0).toLongLong();
		}
	}
	if(newData.type == TYPE_INCOME && newData.isMonthStart) {
		return DATA_RELOAD_NEEDED;
	}
	return 0;
}

qlonglong Household::editData(const Data& oldData, const Data& newData) {
	qDebug() << "Household::editData()";
	Table *table = getTable(TABLE_DATA);
	QString queryStr;
	QList<QString> queries;

	// Update accounts.
	if(oldData.aid == newData.aid) {
		// Value, category or something else changed.
		qlonglong valueDiff = oldData.sum - newData.sum;
		if(valueDiff != 0) {
			Account *a = getAccount(oldData.aid);
			if(oldData.type == TYPE_INCOME) {
				a->money -= valueDiff;
			} else if (oldData.type == TYPE_EXPENSE) {
				a->money += valueDiff;
			}
			queryStr = buildUpdateQueryStr(getTable(TABLE_ACCOUNTS), a->name + COMMA + QString::number(a->money) + COMMA + a->description + COMMA + QString::number(a->uid) \
										   + COMMA + QString::number(a->isDefault), a->id);
			queries.append(queryStr);
		}
	} else {
		// Account changed.
		Account* a = getAccount(oldData.aid);
		if(oldData.type == TYPE_INCOME) {
			a->money -= oldData.sum;
		} else if(oldData.type == TYPE_EXPENSE) {
			a->money += oldData.sum;
		}
		queryStr = buildUpdateQueryStr(getTable(TABLE_ACCOUNTS), a->name + COMMA + QString::number(a->money) + COMMA + a->description + COMMA + QString::number(a->uid) \
									   + COMMA + QString::number(a->isDefault), a->id);
		queries.append(queryStr);

		a = getAccount(newData.aid);
		if(newData.type == TYPE_INCOME) {
			a->money += newData.sum;
		} else if(newData.type == TYPE_EXPENSE) {
			a->money -= newData.sum;
		}
		queryStr = buildUpdateQueryStr(getTable(TABLE_ACCOUNTS), a->name + COMMA + QString::number(a->money) + COMMA + a->description + COMMA + QString::number(a->uid) \
									   + COMMA + QString::number(a->isDefault), a->id);
		queries.append(queryStr);
	}

	// Update data.
	queryStr = buildUpdateQueryStr(table, QString::number(newData.type) + COMMA + QString::number(newData.dateTime.toMSecsSinceEpoch()) + COMMA + QString::number(newData.qty) + COMMA \
								   + QString::number(newData.value) + COMMA + QString::number(newData.cid) + COMMA + QString::number(newData.sum) \
								   + COMMA + QString::number(currentUserId()) + COMMA + QString::number(newData.aid) + COMMA + newData.details + COMMA \
								   + (newData.isMonthStart ? "1" : "0"), newData.id);
	queries.append(queryStr);

	if(!executeQueries(queries)) {
		qDebug() << "Failed to edit data.";
		return DATA_ADD_EDIT_NOT_SUCCESSFULL;
	} else {
		loadAccounts();
		if(oldData.type == TYPE_INCOME) {
			if(oldData.dateTime != newData.dateTime) {
				if(oldData.isMonthStart != newData.isMonthStart || newData.isMonthStart) {
					return DATA_RELOAD_NEEDED;
				}
			}
		}
		return newData.id;
	}
}

qlonglong Household::removeData(const Data& dataToRemove) {
	qDebug() << "Household::removeData()";
	Table *table = getTable(TABLE_DATA);
	QList<QString> queries;
	Account* a = getAccount(dataToRemove.aid);
	if(dataToRemove.type == TYPE_INCOME) {
		a->money -= dataToRemove.sum;
	} else if(dataToRemove.type == TYPE_EXPENSE) {
		a->money += dataToRemove.sum;
	}
	// Update accounts.
	queries.append(buildUpdateQueryStr(getTable(TABLE_ACCOUNTS), a->name + COMMA + QString::number(a->money) + COMMA + a->description + COMMA + QString::number(a->uid) \
									   + COMMA + QString::number(a->isDefault), a->id));
	queries.append(buildDeleteQueryStr(table, COLUMN_ID, dataToRemove.id));

	if(!executeQueries(queries)) {
		qDebug() << "Failed to remove data.";
		return DATA_ADD_EDIT_NOT_SUCCESSFULL;
	} else {
		loadAccounts();
		if(dataToRemove.type == TYPE_INCOME && dataToRemove.isMonthStart) {
			return DATA_RELOAD_NEEDED;
		}
		return getDataIndexInList(dataToRemove);
	}
}

int Household::getDataIndexInList(const Data& d) {
	qDebug() << "Household::getDataIndexInList()";
	for(int i = 0; i < _dataList->count(); ++i) {
		if(_dataList->at(i)->id == d.id) {
			return i;
		}
	}
	return -1;
}

bool Household::loadDataList(const QDateTime &dateFrom, const QDateTime &dateTo) {
	qDebug() << "Household::loadDataList()";
	_dateFilterFrom = dateFrom;
	if(_monthStartsAt == MONTH_STARTS_LAST_INCOME) {
		_dateFilterTo = QDateTime::currentDateTime();
	} else {
		_dateFilterTo = dateTo;
	}
	if(dateFrom == QDateTime::fromMSecsSinceEpoch(0)) {
		return true;
	}
	while(!_dataList->isEmpty()) {
		delete _dataList->takeLast();
	}
	QSqlQuery query(*_db);
	QString queryStr = "SELECT * FROM " + TABLE_DATA + " WHERE " + COLUMN_DATETIME.name + GEQ + APO \
			+ QString::number(dateFrom.toMSecsSinceEpoch()) + APO + AND + COLUMN_DATETIME.name + LEQ + APO \
			+ QString::number(dateTo.toMSecsSinceEpoch()) + APO;
	Table* table = getTable(TABLE_DATA);
	if(query.exec(queryStr)) {
		while(query.next()) {
			Data *i = new Data;
			i->id = query.value(table->getColumnId(COLUMN_ID.name)).toLongLong();
			i->cid = query.value(table->getColumnId(COLUMN_CATEGORY_ID.name)).toLongLong();
			i->categoryName = getCategoryName(i->cid);
			i->value = query.value(table->getColumnId(COLUMN_VALUE.name)).toLongLong();
			i->dateTime = QDateTime::fromMSecsSinceEpoch(query.value(table->getColumnId(COLUMN_DATETIME.name)).toLongLong());
			i->qty = query.value(table->getColumnId(COLUMN_QTY.name)).toLongLong();
			i->sum = query.value(table->getColumnId(COLUMN_SUM.name)).toLongLong();
			i->uid = query.value(table->getColumnId(COLUMN_USERID.name)).toLongLong();
			i->aid = query.value(table->getColumnId(COLUMN_ACCOUNT_ID.name)).toLongLong();
			i->accountName = getAccountName(i->aid);
			i->details = query.value(table->getColumnId(COLUMN_DETAILS.name)).toString();
			i->type = query.value(table->getColumnId(COLUMN_DATA_TYPE.name)).toInt();
			i->isMonthStart = query.value(table->getColumnId(COLUMN_MONTH_BEGIN.name)).toBool();
			_dataList->append(i);
		}
		if(_lastSortType == 0) {
			_lastSortType = SORT_DATE | SORT_DESCENDING;
		}
		sortDataList(SORT_DATE, false);
	} else {
		qDebug() << "Failed to get data list:" << queryStr << ", reason:" << query.lastError().text();
		return false;
	}
	return true;
}

QList<Data*>* Household::getDataList() {
	qDebug() << "Household::getDataList()";
	return _dataList;
}

qlonglong Household::getSum(const int type, const QDateTime& dateFrom, const QDateTime& dateTo) {
	qDebug() << "Household::getSum()";
	qlonglong retVal = 0;
	QSqlQuery query(*_db);
	QString queryStr = "SELECT SUM( " + COLUMN_SUM.name + ") FROM " + TABLE_DATA + " WHERE " + COLUMN_DATA_TYPE.name + EQU + APO + QString::number(type) + APO + AND \
			+ COLUMN_DATETIME.name + GEQ + APO + QString::number(dateFrom.toMSecsSinceEpoch()) + APO + AND + COLUMN_DATETIME.name + LEQ + APO \
			+ QString::number(dateTo.toMSecsSinceEpoch()) + APO;
	if(query.exec(queryStr)) {
		if(query.first()) {
			retVal = query.value(0).toLongLong();
		}
	}
	return retVal;
}

qlonglong Household::getIncomeSum(const QDateTime& dateFrom, const QDateTime& dateTo) {
	qDebug() << "Household::getIncomeSum()";
	return getSum(TYPE_INCOME, dateFrom, dateTo);
}

qlonglong Household::getExpensesSum(const QDateTime& dateFrom, const QDateTime& dateTo) {
	qDebug() << "Household::getExpensesSum()";
	return getSum(TYPE_EXPENSE, dateFrom, dateTo);
}

bool Household::isCategoryInUse(const qlonglong id) {
	qDebug() << "Household::isCategoryInUse()";
	QSqlQuery query(*_db);
	QString queryStr = "SELECT * FROM " + TABLE_DATA + " WHERE " + COLUMN_CATEGORY_ID.name + EQU + APO + id + APO;
	if(query.exec(queryStr)) {
		if(query.next()) {
			return true;
		}
	} else {
		qDebug() << "Failed to check category:" << queryStr << ", reason:" << query.lastError().text();
		return true;
	}
	return false;
}

QString Household::getParentName(const QString& category) {
	qDebug() << "Household::getParentName()";
	for(int i = 0; i < _categories->count(); ++i) {
		if(_categories->at(i)->name == category) {
			return getCategoryName(_categories->at(i)->pid);
		}
	}
	return QString("");
}

bool ascendingCategory(const Data *d1, const Data *d2) {
	return d1->categoryName < d2->categoryName;
}

bool descendingCategory(const Data *d1, const Data *d2) {
	return d1->categoryName > d2->categoryName;
}

bool ascendingDate(const Data *d1, const Data *d2) {
	return d1->dateTime < d2->dateTime;
}

bool descendingDate(const Data *d1, const Data *d2) {
	return d1->dateTime > d2->dateTime;
}

bool ascendingQty(const Data *d1, const Data *d2) {
	return d1->qty < d2->qty;
}

bool descendingQty(const Data *d1, const Data *d2) {
	return d1->qty > d2->qty;
}

bool ascendingSum(const Data *d1, const Data *d2) {
	return d1->sum < d2->sum;
}

bool descendingSum(const Data *d1, const Data *d2) {
	return d1->sum > d2->sum;
}

bool ascendingValue(const Data *d1, const Data *d2) {
	return d1->value < d2->value;
}

bool descendingValue(const Data *d1, const Data *d2) {
	return d1->value > d2->value;
}

void Household::setLastSortType(const int type) {
	qDebug() << "Household::setLastSortType()";
	_lastSortType = type;
}

void Household::sortDataList(const int by, const bool changeOrder, QList<Data*>* dataList, const int sortType) {
	qDebug() << "Household::sortDataList()";
	QList<Data*> *list = 0;
	int lastSortType = 0;
	if(dataList == 0) {
		list = _dataList;
		lastSortType = _lastSortType;
	} else {
		list = dataList;
		lastSortType = sortType;
	}
	if(!changeOrder) {
		if(lastSortType & SORT_ASCENDING) {
			lastSortType &= ~SORT_ASCENDING;
			lastSortType |= SORT_DESCENDING;
		} else if(lastSortType & SORT_DESCENDING) {
			lastSortType &= ~SORT_DESCENDING;
			lastSortType |= SORT_ASCENDING;
		}
	}
	switch(by) {
	case SORT_DATE: // Date
		if(lastSortType & SORT_DATE) {
			if(lastSortType & SORT_DESCENDING) {
				qSort(list->begin(), list->end(), ascendingDate);
				lastSortType = SORT_DATE | SORT_ASCENDING;
			} else if(lastSortType & SORT_ASCENDING) {
				qSort(list->begin(), list->end(), descendingDate);
				lastSortType = SORT_DATE | SORT_DESCENDING;
			}
		} else {
			qSort(list->begin(), list->end(), ascendingDate);
			lastSortType = SORT_DATE | SORT_ASCENDING;
		}
		break;
	case SORT_QTY: // Qty
		if(lastSortType & SORT_QTY) {
			if(lastSortType & SORT_DESCENDING) {
				qSort(list->begin(), list->end(), ascendingQty);
				lastSortType = SORT_QTY | SORT_ASCENDING;
			} else if(lastSortType & SORT_ASCENDING) {
				qSort(list->begin(), list->end(), descendingQty);
				lastSortType = SORT_QTY | SORT_DESCENDING;
			}
		} else {
			qSort(list->begin(), list->end(), ascendingQty);
			lastSortType = SORT_QTY | SORT_ASCENDING;
		}
		break;
	case SORT_VALUE: // Value
		if(lastSortType & SORT_VALUE) {
			if(lastSortType & SORT_DESCENDING) {
				qSort(list->begin(), list->end(), ascendingValue);
				lastSortType = SORT_VALUE | SORT_ASCENDING;
			} else if(lastSortType & SORT_ASCENDING) {
				qSort(list->begin(), list->end(), descendingValue);
				lastSortType = SORT_VALUE | SORT_DESCENDING;
			}
		} else {
			qSort(list->begin(), list->end(), ascendingValue);
			lastSortType = SORT_VALUE | SORT_ASCENDING;
		}
		break;
	case SORT_SUM: // Sum
		if(lastSortType & SORT_SUM) {
			if(lastSortType & SORT_DESCENDING) {
				qSort(list->begin(), list->end(), ascendingSum);
				lastSortType = SORT_SUM | SORT_ASCENDING;
			} else if(lastSortType & SORT_ASCENDING) {
				qSort(list->begin(), list->end(), descendingSum);
				lastSortType = SORT_SUM | SORT_DESCENDING;
			}
		} else {
			qSort(list->begin(), list->end(), ascendingSum);
			lastSortType = SORT_SUM | SORT_ASCENDING;
		}
		break;
	case SORT_CATEGORY: // Category
		if(lastSortType & SORT_CATEGORY) {
			if(lastSortType & SORT_DESCENDING) {
				qSort(list->begin(), list->end(), ascendingCategory);
				lastSortType = SORT_CATEGORY | SORT_ASCENDING;
			} else if(lastSortType & SORT_ASCENDING) {
				qSort(list->begin(), list->end(), descendingCategory);
				lastSortType = SORT_CATEGORY | SORT_DESCENDING;
			}
		} else {
			qSort(list->begin(), list->end(), ascendingCategory);
			lastSortType = SORT_CATEGORY | SORT_ASCENDING;
		}
		break;
	}
}

QList<QString> Household::getChildCategoryNames(const QString& category) {
	qDebug() << "Household::getChildCategories()";
	QSqlQuery query(*_db);
	QString queryStr = "SELECT " + COLUMN_NAME.name + " FROM " + TABLE_CATEGORIES + " WHERE " + COLUMN_PID.name + EQU + APO + QString::number(getCategoryId(category.trimmed())) + APO;
	QList<QString> childCategoryNames;
	if(query.exec(queryStr)) {
		while(query.next()) {
			childCategoryNames.append(query.value(0).toString());
		}
	} else {
		qDebug() << "Failed to retrieve child categories:" << queryStr << ", reason:" << query.lastError().text();
	}
	return childCategoryNames;
}

QList<qlonglong> Household::getChildCategoryIds(const QString& category) {
	qDebug() << "Household::getChildCategories()";
	QSqlQuery query(*_db);
	QString queryStr = "SELECT " + COLUMN_ID.name + " FROM " + TABLE_CATEGORIES + " WHERE " + COLUMN_PID.name + EQU + APO + QString::number(getCategoryId(category.trimmed())) + APO;
	QList<qlonglong> childCategoryIds;
	if(query.exec(queryStr)) {
		while(query.next()) {
			childCategoryIds.append(query.value(0).toLongLong());
		}
	} else {
		qDebug() << "Failed to retrieve child categories:" << queryStr << ", reason:" << query.lastError().text();
	}
	return childCategoryIds;
}

bool Household::addUser(const User& user) {
	qDebug() << "Household::addUser()";
	QCryptographicHash pwdHash(QCryptographicHash::Md5);
	pwdHash.addData(user.password.toUtf8());
	QString queryStr = buildInsertQueryStr(getTable(TABLE_USERS), user.name + COMMA + QString(pwdHash.result()) + COMMA + user.description + COMMA + QString::number(user.level));
	if(executeQuery(queryStr)) {
		loadUsers();
		return true;
	} else {
		qDebug() << "Failed to add user.";
		return false;
	}
}

bool Household::login() {
	qDebug() << "Household::login()";
	LoginDialog dlg;
	if(dlg.exec() == QDialog::Accepted) {
		if(qlonglong uid = verifyPassword(dlg.getName(), dlg.getPassword()) != -1) {
			_currentUser = getUser(uid);
			return true;
		}
	}
	return false;
}

void Household::setCurrentUser(const QString& name) {
	qDebug() << "Household::setCurrentUser()";
	_currentUser = getUser(getUserId(name));
}

qlonglong Household::currentUserId() {
	qDebug() << "Household::currentUserId()";
	return _currentUser->id;
}

int Household::currentUserAccessLevel() {
	qDebug() << "Household::currentUserAccessLevel()";
	return _currentUser->level;
}

QString Household::getUserName(const qlonglong id) {
	qDebug() << "Household::getUserName()";
	for(int i = 0; i < _users.count(); ++i) {
		if(_users[i]->id == id) {
			return _users[i]->name;
		}
	}
	return "";
}

QString Household::getUserPassword(const qlonglong id) {
	qDebug() << "Household::getUserPassword()";
	for(int i = 0; i < _users.count(); ++i) {
		if(_users[i]->id == id) {
			return _users[i]->password;
		}
	}
	return "";
}

QString	Household::getUserDescription(const qlonglong id) {
	qDebug() << "Household::getUserDescription()";
	for(int i = 0; i < _users.count(); ++i) {
		if(_users[i]->id == id) {
			return _users[i]->description;
		}
	}
	return "";
}

int	Household::getUserAccessLevel(const qlonglong id) {
	qDebug() << "Household::getUserAccessLevel()";
	for(int i = 0; i < _users.count(); ++i) {
		if(_users[i]->id == id) {
			return _users[i]->level;
		}
	}
	return -1;
}


qlonglong Household::getUserId(const QString& name) {
	qDebug() << "Household::getUserId()";
	for(int i = 0; i < _users.count(); ++i) {
		if(_users[i]->name == name) {
			return _users[i]->id;
		}
	}
	return -1;
}

QList<User*> Household::getUserList() {
	qDebug() << "Household::getUserList()";
	return _users;
}

bool Household::editUser(const User& user) {
	qDebug() << "Household::editUser()";
	QString queryStr = buildUpdateQueryStr(getTable(TABLE_USERS), user.name + COMMA + user.password + COMMA + user.description + COMMA + QString::number(user.level), user.id);
	if(executeQuery(queryStr)) {
		loadUsers();
		return true;
	} else {
		qDebug() << "Failed to edit user.";
		return false;
	}
}

bool Household::removeUser(const qlonglong id) {
	qDebug() << "Household::removeUser()";
	if(isUserLastAdmin(id)) {
		QMessageBox::information(this, tr("Information"), tr("Unable to remove user. User is the last administrator of this household."));
		return false;
	}
	if(isUserInUse(id)) {
		QMessageBox::information(this, tr("Information"), tr("Unable to remove user. User is in use."));
		return false;
	}
	QString queryStr = buildDeleteQueryStr(getTable(TABLE_USERS), COLUMN_ID, id);//"DELETE FROM " + TABLE_USERS + " WHERE " + COLUMN_ID.name + EQU + APO + QString::number(id) + APO;
	if(executeQuery(queryStr)) {
		loadUsers();
		return true;
	} else {
		qDebug() << "Failed to remove user.";
		return false;
	}
}

User* Household::getUser(const qlonglong id) {
	qDebug() << "Household::getUser()";
	for(int i = 0; i < _users.count(); ++i) {
		if(_users[i]->id == id) {
			return _users[i];
		}
	}
	return 0;
}

bool Household::isUserInUse(const qlonglong id) {
	qDebug() << "Household::isUserInUse()";
	if(_users.count() == 1) {
		return true;
	}
	QSqlQuery query(*_db);

	// Check data.
	QString queryStr = "SELECT * FROM " + TABLE_DATA + " WHERE " + COLUMN_USERID.name + EQU + APO + QString::number(id) + APO;
	if(query.exec(queryStr)) {
		if(query.next()) {
			return true;
		}
	} else {
		return true;
	}

	// Check categories.
	queryStr = "SELECT * FROM " + TABLE_CATEGORIES + " WHERE " + COLUMN_USERID.name + EQU + APO + QString::number(id) + APO;
	if(query.exec(queryStr)) {
		if(query.next()) {
			return true;
		}
	}  else {
		return true;
	}

	// Check accounts.
	queryStr = "SELECT * FROM " + TABLE_ACCOUNTS + " WHERE " + COLUMN_USERID.name + EQU + APO + QString::number(id) + APO;
	if(query.exec(queryStr)) {
		if(query.next()) {
			return true;
		}
	}  else {
		return true;
	}

	// Check transfers.
	queryStr = "SELECT * FROM " + TABLE_TRANSFERS + " WHERE " + COLUMN_USERID.name + EQU + APO + QString::number(id) + APO;
	if(query.exec(queryStr)) {
		if(query.next()) {
			return true;
		}
	}  else {
		return true;
	}
	return false;
}

bool Household::isUserLastAdmin(const qlonglong id) {
	qDebug() << "Household::isUserLastAdmin()";
	QSqlQuery query(*_db);
	QString queryStr = "SELECT * FROM " + TABLE_USERS + " WHERE " + COLUMN_ACCESS_LEVEL.name + EQU + APO + QString::number(ACCESS_ADMIN) + APO + AND \
			+ COLUMN_ID.name + NEQ + APO + QString::number(id) + APO;
	if(query.exec(queryStr)) {
		if(query.next()) {
			return false;
		}
	}
	return true;
}

QString Household::getAccountName(const qlonglong id) {
	qDebug() << "Household::getAccountName()";
	for(int i = 0; i < _accounts.count(); ++i) {
		if(_accounts[i]->id == id) {
			return _accounts[i]->name;
		}
	}
	return "";
}

qlonglong Household::getAccountId(const QString &name) {
	qDebug() << "Household::getAccountId()";
	for(int i = 0; i < _accounts.count(); ++i) {
		if(_accounts[i]->name == name) {
			return _accounts[i]->id;
		}
	}
	return -1;
}

QList<Account*> Household::getAccountList() {
	qDebug() << "Household::getAccountList()";
	return _accounts;
}

bool Household::addAccount(const Account& acc) {
	qDebug() << "Household::addAccount()";
	QString queryStr = buildInsertQueryStr(getTable(TABLE_ACCOUNTS), acc.name + COMMA + QString::number(acc.money) + COMMA + acc.description + COMMA + QString::number(acc.uid) \
										   + COMMA + QString::number(acc.isDefault));
	if(executeQuery(queryStr)) {
		loadAccounts();
		return true;
	} else {
		qDebug() << "Failed to add account.";
		return false;
	}
}

bool Household::editAccount(const Account& acc) {
	qDebug() << "Household::editAccount()";
	QString queryStr = buildUpdateQueryStr(getTable(TABLE_ACCOUNTS),  acc.name + COMMA + QString::number(acc.money) + COMMA + acc.description + COMMA + QString::number(acc.uid) \
										   + COMMA + QString::number(acc.isDefault), acc.id);
	if(executeQuery(queryStr)) {
		loadAccounts();
		return true;
	} else {
		qDebug() << "Failed to edit account.";
		return false;
	}
}

bool Household::removeAccount(const qlonglong id) {
	qDebug() << "Household::removeAccount()";
	if(isAccountInUse(id)) {
		QMessageBox::information(this, tr("Information"), tr("Unable to remove account. Account is in use."));
		return false;
	}
	QString queryStr = buildDeleteQueryStr(getTable(TABLE_ACCOUNTS), COLUMN_ID, id);//"DELETE FROM " + TABLE_ACCOUNTS + " WHERE " + COLUMN_ID.name + EQU + APO + QString::number(id) + APO;
	if(executeQuery(queryStr)) {
		loadAccounts();
		return true;
	} else {
		qDebug() << "Failed to remove account.";
		return false;
	}
}

bool Household::isAccountInUse(const qlonglong id) {
	qDebug() << "Household::isAccountInUse()";
	if(_accounts.count() == 1) {
		return true;
	}
	QSqlQuery query(*_db);

	// Check data.
	QString queryStr = "SELECT * FROM " + TABLE_DATA + " WHERE " + COLUMN_ACCOUNT_ID.name + EQU + APO + QString::number(id) + APO;
	if(query.exec(queryStr)) {
		if(query.next()) {
			return true;
		}
	} else {
		return true;
	}

	// Check transfers from.
	queryStr = "SELECT * FROM " + TABLE_TRANSFERS + " WHERE " + COLUMN_FROM_ID.name + EQU + APO + QString::number(id) + APO;
	if(query.exec(queryStr)) {
		if(query.next()) {
			return true;
		}
	} else {
		return true;
	}

	// Check transfers to.
	queryStr = "SELECT * FROM " + TABLE_TRANSFERS + " WHERE " + COLUMN_TO_ID.name + EQU + APO + QString::number(id) + APO;
	if(query.exec(queryStr)) {
		if(query.next()) {
			return true;
		}
	} else {
		return true;
	}

	return false;
}

bool Household::loadAccounts() {
	qDebug() << "Household::loadAccounts()";
	while(!_accounts.isEmpty()) {
		delete _accounts.takeLast();
	}
	QSqlQuery query(*_db);
	QString queryStr = "SELECT * FROM " + TABLE_ACCOUNTS;
	if(query.exec(queryStr)) {
		while(query.next()) {
			Account* a = new Account;
			a->id = query.value(getTable(TABLE_ACCOUNTS)->getColumnId(COLUMN_ID.name)).toLongLong();
			a->name = query.value(getTable(TABLE_ACCOUNTS)->getColumnId(COLUMN_NAME.name)).toString();
			a->description = query.value(getTable(TABLE_ACCOUNTS)->getColumnId(COLUMN_DETAILS.name)).toString();
			a->uid = query.value(getTable(TABLE_ACCOUNTS)->getColumnId(COLUMN_USERID.name)).toLongLong();
			a->money = query.value(getTable(TABLE_ACCOUNTS)->getColumnId(COLUMN_VALUE.name)).toLongLong();
			a->isDefault = query.value(getTable(TABLE_ACCOUNTS)->getColumnId(COLUMN_DEFAULT.name)).toInt();
			_accounts.append(a);
		}
	} else {
		// TBD - failed to load account list.
		return false;
	}
	return true;
}

Account* Household::getAccount(const qlonglong id) {
	qDebug() << "Household::getAccount()";
	for(int i = 0; i < _accounts.count(); ++i) {
		if(_accounts[i]->id == id) {
			return _accounts[i];
		}
	}
	return 0;
}

bool Household::isAccountDefault(const qlonglong id) {
	qDebug() << "Household::isAccountDefault()";
	if(getAccount(id)->isDefault == ACCOUNT_DEFAULT) {
		return true;
	} else {
		return false;
	}
}

qlonglong Household::getDefaultAccountId() {
	qDebug() << "Household::getDefaultAccountId()";
	for(int i = 0; i < _accounts.count(); ++i) {
		if(_accounts[i]->isDefault == ACCOUNT_DEFAULT) {
			return _accounts[i]->id;
		}
	}
	return -1;
}

qlonglong Household::getAccountMoney(const qlonglong id) {
	qDebug() << "Household::getAccountMoney()";
	for(int i = 0; i < _accounts.count(); ++i) {
		if(_accounts[i]->id == id) {
			return _accounts[i]->money;
		}
	}
	return -1;
}

QString Household::getAccountDescription(const qlonglong id) {
	qDebug() << "Household::getAccountDescription()";
	for(int i = 0; i < _accounts.count(); ++i) {
		if(_accounts[i]->id == id) {
			return _accounts[i]->description;
		}
	}
	return "";
}

qlonglong Household::getAccountUserId(const qlonglong id) {
	qDebug() << "Household::getAccountMoney()";
	for(int i = 0; i < _accounts.count(); ++i) {
		if(_accounts[i]->id == id) {
			return _accounts[i]->uid;
		}
	}
	return -1;
}

QList<QString> Household::getAccountNames() {
	qDebug() << "Household::getAccountNames()";
	QList<QString> accountNames;
	for(int i = 0; i < _accounts.count(); ++i) {
		accountNames.append(_accounts[i]->name);
	}
	return accountNames;
}

int Household::getAccountCount() {
	qDebug() << "Household::getAccountCount()";
	return _accounts.count();
}

int Household::verifyPassword(const QString& username, const QString& password) {
	qDebug() << "Household::verifyPassword()";
	for(int i = 0; i < _users.count(); ++i) {
		if(_users[i]->name == username) {
			QCryptographicHash pwdHash(QCryptographicHash::Md5);
			pwdHash.addData(password.toUtf8());
			if(_users[i]->password == QString(pwdHash.result())) {
				return _users[i]->id;
			}
		}
	}
	return -1;
}

void Household::changeUserPassword(const User &user) {
	qDebug() << "Household::changeUserPassword()";
	if(!editUser(user)) {
		QMessageBox::information(this, tr("Error"), tr("Failed to change password."));
		return;
	}
}

QDateTime Household::getLastIncomeDate() {
	qDebug() << "Household::getLastIncomeDate()";
	QDateTime lastIncomeDate;
	QSqlQuery query(*_db);
	QString queryStr = "SELECT MAX(" + COLUMN_DATETIME.name + ") FROM " + TABLE_DATA + " WHERE " + COLUMN_DATA_TYPE.name + EQU + APO + QString::number(TYPE_INCOME) + APO \
				+ AND + COLUMN_MONTH_BEGIN.name + EQU + APO + QString::number(1) + APO;
	if(query.exec(queryStr)) {
		if(query.first()) {
			lastIncomeDate = QDateTime::fromMSecsSinceEpoch(query.value(0).toLongLong());
		}
	}
	return QDateTime(lastIncomeDate);
}
/*
QDateTime Household::getPreviousIncomeDate() {
	qDebug() << "Household::getPreviousIncomeDate()";
	QDateTime lastIncomeDate;
	QSqlQuery query(*_db);
	QString queryStr = "SELECT MAX(" + COLUMN_DATETIME.name + ") FROM " + TABLE_DATA + " WHERE " + COLUMN_DATA_TYPE.name + EQU + APO + QString::number(TYPE_INCOME) + APO;
	if(query.exec(queryStr)) {
		if(query.first()) {
			queryStr = "SELECT " + COLUMN_DATETIME.name + " FROM " + TABLE_DATA + " WHERE " + COLUMN_DATA_TYPE.name + EQU + APO + QString::number(TYPE_INCOME) + APO + AND \
					+ COLUMN_DATETIME.name + LT + APO + query.value(0).toString() + APO + AND + ;
			lastIncomeDate = QDateTime::fromMSecsSinceEpoch(query.value(0).toLongLong());
		}
	}
	return QDateTime(lastIncomeDate);
}
*/
bool Household::setDefaultAccount(const Account& acc) {
	qDebug() << "Household::setDefaultAccount()";
	QList<QString> queries;
	Account *defAccount = getAccount(getDefaultAccountId());
	defAccount->isDefault = ACCOUNT_NORMAL;
	QString queryStr = buildUpdateQueryStr(getTable(TABLE_ACCOUNTS),  defAccount->name + COMMA + QString::number(defAccount->money) + COMMA + defAccount->description + COMMA \
										   + QString::number(defAccount->uid) + COMMA + QString::number(defAccount->isDefault), defAccount->id);
	queries.append(queryStr);

	queryStr = buildUpdateQueryStr(getTable(TABLE_ACCOUNTS),  acc.name + COMMA + QString::number(acc.money) + COMMA + acc.description + COMMA + QString::number(acc.uid) \
											   + COMMA + QString::number(acc.isDefault), acc.id);
	queries.append(queryStr);

	if(!executeQueries(queries)) {
		qDebug() << "Failed to set account as default.";
		return false;
	} else {
		loadAccounts();
		return true;
	}
}

bool Household::transferMoneyBetweenAccounts(const qlonglong fromId, const qlonglong toId, const qlonglong value) {
	qDebug() << "Household::transferMoneyBetweenAccounts()";
	QList<QString> queries;
	Account *acc = getAccount(fromId);
	acc->money -= value;
	QString queryStr = buildUpdateQueryStr(getTable(TABLE_ACCOUNTS),  acc->name + COMMA + QString::number(acc->money) + COMMA + acc->description + COMMA \
					   + QString::number(acc->uid) + COMMA + QString::number(acc->isDefault), acc->id);
	queries.append(queryStr);

	acc = getAccount(toId);
	acc->money += value;
	queryStr = buildUpdateQueryStr(getTable(TABLE_ACCOUNTS),  acc->name + COMMA + QString::number(acc->money) + COMMA + acc->description + COMMA \
			   + QString::number(acc->uid) + COMMA + QString::number(acc->isDefault), acc->id);
	queries.append(queryStr);

	queryStr = buildInsertQueryStr(getTable(TABLE_TRANSFERS), QString::number(fromId) + COMMA + QString::number(toId) + COMMA + QString::number(value) \
			   + COMMA + QString::number(currentUserId()));
	queries.append(queryStr);

	if(!executeQueries(queries)) {
		qDebug() << "Failed to transfer money.";
		return false;
	} else {
		loadAccounts();
		return true;
	}
}

Data* Household::getData(const qlonglong id) {
	qDebug() << "Household::getData()";
	QList<Data*>* list = _dataList;
	for(int i = 0; i < list->count(); ++i) {
		if(list->at(i)->id == id) {
			return list->at(i);
		}
	}
	return 0;
}

QDateTime Household::getDateFilterFrom() {
	qDebug() << "Household::getDateFilterFrom()";
	return _dateFilterFrom;
}

QDateTime Household::getDateFilterTo() {
	qDebug() << "Household::getDateFilterTo()";
	return _dateFilterTo;
}

int Household::getMonthStartsAt() {
	qDebug() << "Household::getMonthStartsAt()";
	return _monthStartsAt;
}

int Household::getLastSortType() {
	qDebug() << "Household::getLastSortType()";
	return _lastSortType;
}

int Household::getColumnVisibility() {
	qDebug() << "Household::getColumnVisibility()";
	return _columnVisibility;
}

void Household::setColumnVisibility(const int v) {
	qDebug() << "Household::setColumnVisibility()";
	_columnVisibility = v;
}

QList<Data*>* Household::getChartData(const QDateTime& timeFrom, const QDateTime& timeTo) {
	QList<Data*>* list = new QList<Data*>;
	for(int i = 0; i < _dataList->count(); ++i) {
		Data* d = _dataList->at(i);
		if(d->dateTime < timeFrom) {
			continue;
		}
		bool found = false;
		for(int j = 0; j < list->count(); ++j) {
			Data* cd = list->at(j);
			if(cd->dateTime.date().weekNumber() == d->dateTime.date().weekNumber()) {
				cd->value += d->sum;
				found = true;
				break;
			}
		}
		if(!found) {
			Data* ncd = new Data;
			ncd->dateTime = d->dateTime;
			ncd->value += d->sum;
			list->append(ncd);
		}
	}
	sortDataList(SORT_DATE, false, list);
	return list;
}
