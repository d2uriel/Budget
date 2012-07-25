#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QString>
#include <QTreeWidgetItem>
#include <QDateTime>

typedef struct User {
	qlonglong id;
	QString name;
	QString password;
	QString description;
	int level;

	User() {
		id = -1;
		name = "";
		password = "";
		description = "";
		level = -1;
	}
} User;

typedef struct Category {
	qlonglong id;
	qlonglong pid;
	QString name;
	int type;
	qlonglong uid;
} Category;

typedef struct Account {
	qlonglong id;
	qlonglong uid;
	QString name;
	QString description;
	qlonglong money;
	int isDefault;
} Account;

typedef struct ChartData {
	QDateTime dateTime;
	qlonglong value;

	ChartData() {
		value = 0;
	}
} ChartData;

class Data : public QTreeWidgetItem {
public:
	qlonglong id;
	qlonglong cid;
	qlonglong qty;
	qlonglong value;
	qlonglong sum;
	QDateTime dateTime;
	qlonglong uid;
	QString details;
	qlonglong aid;
	QString categoryName;
	QString accountName;
	int type;
	bool isMonthStart;

	Data() {
		id = 0; cid = 0; qty = 0; value = 0; sum = 0; uid = 0; aid = 0; type = -1; isMonthStart = false;
	}

	void fromData(const Data& d) {
		id = d.id; cid = d.cid; qty = d.qty; value = d.value; sum = d.sum; dateTime = d.dateTime; uid = d.uid; details = d.details; aid = d.aid; categoryName = d.categoryName; accountName = d.accountName; type = d.type; isMonthStart = d.isMonthStart;
	}
};


const qlonglong DB_VERSION = 1;
const QString VERSION = "0.7";
const QString VERSION_STR = "VERSION";

typedef struct COLUMN {
	QString name;
	QString type;
} COLUMN;

const QString DB_EXT = ".hhb";
const QString CAT_EXT = ".cat";
const QString COMMA = ", ";
const QString SPACE = " ";
const QString APO = "'";
const QString EQU = " = ";
const QString AND = " AND ";
const QString NEQ = " <> ";
const QString GT = " > ";
const QString GEQ = " >= ";
const QString LT = " < ";
const QString LEQ = " <= ";
const QString APOCOMMAAPO = APO + COMMA + APO;

const QString SETTINGS = "settings.ini";
const QString SETTINGS_LANGUAGE = "LANGUAGE";

// Error numbers.
const qlonglong DATA_RELOAD_NEEDED				= -2;
const qlonglong DATA_ADD_EDIT_NOT_SUCCESSFULL	= -1;
const int NO_ERROR								= 0;
const int MISSING_FILENAME_OR_PATH				= 1;
const int FAILED_TO_CREATE_TABLE				= 2;
const int FAILED_TO_CREATE_OPEN_DB				= 3;

// Column types.
const QString TYPE_TEXT = "TEXT";
const QString TYPE_INTEGER = "INTEGER";
const QString TYPE_INTEGER_PRIMARY_KEY_ASC = "INTEGER PRIMARY KEY ASC";

// SQL table names.
const QString TABLE_HOUSEHOLD = "THousehold";
const QString TABLE_CATEGORIES = "TCategories";
const QString TABLE_DATA = "TData";
const QString TABLE_ACCOUNTS = "TAccounts";
const QString TABLE_CONFIG = "TConfig";
const QString TABLE_USERS = "TUsers";
const QString TABLE_TRANSFERS = "TTransfers";

// SQL column names.
const COLUMN COLUMN_NAME = {"Cname", TYPE_TEXT};
const COLUMN COLUMN_USERID = {"CuserId", TYPE_INTEGER};
const COLUMN COLUMN_DATETIME = {"Cdatetime", TYPE_INTEGER};
const COLUMN COLUMN_ID = {"Cid", TYPE_INTEGER_PRIMARY_KEY_ASC};
const COLUMN COLUMN_PID = {"Cpid", TYPE_INTEGER};
const COLUMN COLUMN_TYPE = {"Ctype", TYPE_TEXT};
const COLUMN COLUMN_CATEGORY_ID = {"CcategoryId", TYPE_INTEGER};
const COLUMN COLUMN_QTY = {"Cqty", TYPE_INTEGER};
const COLUMN COLUMN_VALUE = {"Cvalue", TYPE_INTEGER};
const COLUMN COLUMN_SUM = {"Csum", TYPE_INTEGER};
const COLUMN COLUMN_VERSION = {"Cversion", TYPE_INTEGER};
const COLUMN COLUMN_DETAILS = {"Cdetails", TYPE_TEXT};
const COLUMN COLUMN_ACCESS_LEVEL = {"CaccessLevel", TYPE_INTEGER};
const COLUMN COLUMN_PASSWORD = {"Cpassword", TYPE_TEXT};
const COLUMN COLUMN_ACCOUNT_ID = {"CaccountId", TYPE_INTEGER};
const COLUMN COLUMN_DEFAULT = {"Cdefault", TYPE_INTEGER};
const COLUMN COLUMN_FROM_ID = {"Cfid", TYPE_INTEGER};
const COLUMN COLUMN_TO_ID = {"Ctid", TYPE_INTEGER};
const COLUMN COLUMN_DATA_TYPE = {"CdataType", TYPE_INTEGER};
const COLUMN COLUMN_MONTH_BEGIN = {"CmonthBegin", TYPE_INTEGER};

const int TYPE_INCOME = 0;
const int TYPE_EXPENSE = 1;
const int TYPE_ALL = 2;

const int ACCOUNT_DEFAULT = 1;
const int ACCOUNT_NORMAL = 2;

// Access levels.
const int ACCESS_ADMIN = 999;
const int ACCESS_USER = 0;

// Tree widgets column numbers.
const int DATATREE_DATE = 0;
const int DATATREE_QTY = 1;
const int DATATREE_VALUE = 2;
const int DATATREE_SUM = 3;
const int DATATREE_CATEGORY = 4;
const int DATATREE_USERNAME = 5;
const int DATATREE_DETAILS = 6;
const int DATATREE_ID = 7;

// Above column visibility settings.
const int DT_DATE = 1;
const int DT_QTY = 2;
const int DT_VAL = 4;
const int DT_SUM = 8;
const int DT_CAT = 16;
const int DT_USER = 32;
const int DT_DETAIL = 64;
const int DT_ID = 128;
const int DT_ALL = DT_DATE | DT_QTY | DT_VAL | DT_SUM | DT_CAT | DT_USER | DT_DETAIL | DT_ID;

const int USERSTREE_NAME = 0;
const int USERSTREE_DESC = 1;
const int USERSTREE_LEVEL = 2;
const int USERSTREE_ID = 3;

const int ACCOUNTTREE_NAME = 0;
const int ACCOUNTTREE_MONEY = 1;
const int ACCOUNTTREE_DESC = 2;
const int ACCOUNTTREE_ID = 3;

// Sort types.
const int SORT_DATE = 1;
const int SORT_CATEGORY = 2;
const int SORT_VALUE = 4;
const int SORT_QTY = 8;
const int SORT_SUM = 16;
const int SORT_ASCENDING = 32;
const int SORT_DESCENDING = 64;

// Month starts at options.
const int MONTH_STARTS_LAST_INCOME = 1;
const int MONTH_STARTS_FIRST_DAY = 2;

// Chart constants.
const int MIN_CHART_WIDTH = 300;
const int MIN_CHART_HEIGHT = 300;
const int MIN_SPACING_X_AXIS = 10;
const int MIN_SPACING_Y_AXIS = 50;
const int CHART_MARGIN = 100;
const int CHART_GROUPING_DAY = 1;
const int CHART_GROUPING_WEEK = 2;
const int CHART_GROUPING_MONTH = 3;
#endif // CONSTANTS_H
