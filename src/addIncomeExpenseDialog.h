#ifndef ADDINCOMEEXPENSE_H
#define ADDINCOMEEXPENSE_H

#include <QDialog>
#include <QDateTime>
#include "constants.h"

namespace Ui {
	class addIncomeExpense;
}

class AddIncomeExpenseDialog : public QDialog {
	Q_OBJECT
	
public:
	// Members.

	// Methods.
	explicit AddIncomeExpenseDialog(const int type, QWidget *parent = 0);
	~AddIncomeExpenseDialog();

	void					setShowHelpText(bool show = true);

	void					fillCategoriesComboBox(const QList<QString> cn);
	void					fillAccountsComboBox(const QList<QString> acc);

	void					setValue(const QString& v);
	QString					getValue();

	void					setCategoryName(const QString& name);
	QString					getCategoryName();

	void					setDateTime(const QDateTime& dt);
	QDateTime				getDateTime();

	void					setQty(const QString& q);
	QString					getQty();

	void					setAccount(const QString& acc);
	QString					getAccount();

	void					setDetails(const QString& details);
	QString					getDetails();

	bool					getIsMonthStart();
	void					setIsMonthStart(const bool isIt);

	void					hideAccounts();
	void					showAccounts();

	
private:
	// Members.
	Ui::addIncomeExpense	*ui;
	int						_type;

	// Methods.

public slots:
	int						add();
	int						edit();

private slots:
	void					verifyDialog();
	void					onValuesChanged();

signals:
	void					addCategoryRequested();
};

#endif // ADDINCOMEEXPENSE_H
