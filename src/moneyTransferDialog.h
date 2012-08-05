#ifndef MONEYTRANSFERDIALOG_H
#define MONEYTRANSFERDIALOG_H

#include "household.h"
#include <QDialog>

namespace Ui {
class MoneyTransferDialog;
}

class MoneyTransferDialog : public QDialog {
	Q_OBJECT
	
public:
	explicit MoneyTransferDialog(Household *h, QWidget *parent = 0);
	~MoneyTransferDialog();

	void setShowHelpText(bool show = true);

	void setAccountFrom(const QString& from);
	QString getAccountFrom();

	void setAccountTo(const QString& to);
	QString getAccountTo();

	void fillAccounts(const QList<QString> an);

	QString getValue();
	
private:
	Ui::MoneyTransferDialog *ui;
	Household *_household;

private slots:
	void verifyDialog();
	void swapAccounts();
	void refreshAccountFromMoney(int index);
	void refreshAccountToMoney(int index);
};

#endif // MONEYTRANSFERDIALOG_H
