#ifndef HOUSEHOLD_H
#define HOUSEHOLD_H

#include <QWidget>
#include <QString>
#include <QList>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QDateTime>
#include "constants.h"
#include "table.h"
#include <QTreeWidgetItem>
#include "chart.h"

class Household : public QWidget {
	Q_OBJECT

public:
	Household();
	~Household();

signals:
	void					categoriesChanged();

private:
	// Members.
	QString					_name;
	QString					_path;
	QSqlDatabase*			_db;
	QList<Category*>*		_categories;
	QList<Data*>*			_dataList;
	QList<Table*>*			_tables;
	QList<Account*>			_accounts;
	QList<User*>			_users;
	int						_lastSortType;
	qlonglong				_dbVersion;
	User					*_currentUser;
	QDateTime				_dateFilterFrom;
	QDateTime				_dateFilterTo;
	int						_monthStartsAt;
	int						_columnVisibility;

	// Methods.
	Table*					getTable(const QString& name);
	bool					loadCategories();
	QString					buildInsertQueryStr(Table* const table, const QString& values);
	QString					buildUpdateQueryStr(Table* const table, const QString& values, const qlonglong id);
	QString					buildDeleteQueryStr(Table* const table, const COLUMN& column, const qlonglong id);
	void					setupTables();
	qlonglong				getSum(const int type, const QDateTime& dateFrom, const QDateTime& dateTo);
	bool					isCategoryInUse(const qlonglong id);
	void					setLastSortType(const int type);
	bool					updateDB();
	bool					updateDBv1();
	bool					loadUsers();
	bool					loadAccounts();

public:
	// Members.

	// Methods.
	static qlonglong		qtyIntFromMoneyString(const QString s);
	static qlonglong		valueIntFromMoneyString(const QString s);
	static QString			stringFromValueInt(const qlonglong i);
	static QString			stringFromQtyInt(const qlonglong i);
	static qlonglong		multiplyToRoundedInt(const qlonglong v1, const qlonglong v2);

	void					setName(const QString& n) { _name = n; }
	QString					getName() { return _name; }

	void					setPath(const QString& p) { _path = p; }
	QString					getPath() { return _path; }

	bool					openDB();
	bool					createDB();
	bool					createTables();
	bool					createTable(Table* const table);
	bool					loadDB();
	bool					executeQuery(const QString& queryStr);
	bool					executeQueries(QList<QString> queries);

	const QList<Category*>*	getCategories();
	const QList<Category*>	getParentCategories();
	const QList<QString>	getParentCategoryNames(const int type);
	const QList<Category*>	getChildCategories();
	const QList<QString>	getCategoryNames(const int type);
	bool					addCategory(const Category& c, const bool refresh = true);
	bool					removeCategory(const QString& name);
	bool					editCategory(const Category& c, const qlonglong id);
	bool					refreshCategories();
	qlonglong				getCategoryId(const QString& name);
	QString					getCategoryName(const qlonglong id);
	int						getIncomeCategoryCount();
	int						getExpensesCategoryCount();
	QString					getParentName(const QString& category);
	QList<QString>			getChildCategoryNames(const QString& category);
	QList<qlonglong>		getChildCategoryIds(const QString& category);

	qlonglong				addData(const Data& newData);
	qlonglong				editData(const Data& oldData, const Data& newData);
	qlonglong				removeData(const Data& dataToRemove);
	Data*					getData(const qlonglong id);
	bool					loadDataList(const QDateTime& dateFrom, const QDateTime& dateTo);
	QList<Data*>*			getDataList();
	int						getDataIndexInList(const Data& d);

	QList<Data*>*		getChartData(const QDateTime& timeFrom, const QDateTime& timeTo);

	QSqlDatabase*			db() { return _db; }

	QDateTime				getLastIncomeDate();
	//QDateTime				getPreviousIncomeDate();
	qlonglong				getIncomeSum(const QDateTime& dateFrom, const QDateTime& dateTo);
	qlonglong				getExpensesSum(const QDateTime& dateFrom, const QDateTime& dateTo);

	void					sortDataList(const int by, const bool changeOrder = true, QList<Data*>* dataList = 0, const int sortType = SORT_DATE | SORT_ASCENDING);

	bool					addUser(const User& user);
	bool					editUser(const User& user);
	bool					removeUser(const qlonglong id);
	bool					login();
	qlonglong				currentUserId();
	int						currentUserAccessLevel();
	qlonglong				getUserId(const QString& name);
	QString					getUserName(const qlonglong id);
	QString					getUserPassword(const qlonglong id);
	QString					getUserDescription(const qlonglong id);
	int						getUserAccessLevel(const qlonglong id);
	QList<User*>			getUserList();
	User*					getUser(const qlonglong id);
	bool					isUserInUse(const qlonglong id);
	bool					isUserLastAdmin(const qlonglong id);
	void					setCurrentUser(const QString& name);
	int						verifyPassword(const QString& username, const QString& password);
	void					changeUserPassword(const User& user);

	bool					addAccount(const Account& acc);
	bool					editAccount(const Account& acc);
	bool					removeAccount(const qlonglong id);
	bool					isAccountInUse(const qlonglong id);
	QList<Account*>			getAccountList();
	QList<QString>			getAccountNames();
	qlonglong				getAccountId(const QString& name);
	QString					getAccountName(const qlonglong id);
	qlonglong				getAccountMoney(const qlonglong id);
	QString					getAccountDescription(const qlonglong id);
	qlonglong				getAccountUserId(const qlonglong id);
	int						getAccountCount();
	Account*				getAccount(const qlonglong id);
	bool					isAccountDefault(const qlonglong id);
	qlonglong				getDefaultAccountId();
	bool					setDefaultAccount(const Account& acc);
	bool					transferMoneyBetweenAccounts(const qlonglong fromId, const qlonglong toId, const qlonglong value);

	QDateTime				getDateFilterFrom();
	QDateTime				getDateFilterTo();
	int						getMonthStartsAt();
	int						getLastSortType();
	int						getColumnVisibility();
	void					setColumnVisibility(const int v);
};

#endif // HOUSEHOLD_H
