#include "accountsForm.h"
#include "ui_accountsForm.h"
#include "addEditAccountsDialog.h"
#include <QDebug>
#include <QMenu>
#include <QMessageBox>

AccountsForm::AccountsForm(Household *h, QWidget *parent) : QDialog(parent), ui(new Ui::AccountsForm) {
	ui->setupUi(this);
	_household = h;
	ui->treeWidget_accounts->setColumnHidden(ACCOUNTTREE_ID, true);
	//ui->treeWidget_accounts->setColumnHidden(ACCOUNTTREE_MONEY, true);
	ui->treeWidget_accounts->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(ui->treeWidget_accounts, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
	connect(ui->pushButton_addAccount, SIGNAL(clicked()), this, SLOT(onAddAccountClicked()));
	refreshAccountList();
}

AccountsForm::~AccountsForm() {
	delete ui;
}

void AccountsForm::setShowHelpText(bool show) {
	if(!show) {
		ui->label_helpText->setHidden(true);
	} else {
		ui->label_helpText->setVisible(true);
	}
}

void AccountsForm::clear() {
	qDebug() << "AccountsForm::clear()";
	while(ui->treeWidget_accounts->topLevelItemCount() > 0) {
		delete ui->treeWidget_accounts->takeTopLevelItem(0);
	}
}

void AccountsForm::refreshAccountList() {
	qDebug() << "AccountsForm::refreshAccountList()";
	clear();
	QList<Account*> accounts = _household->getAccountList();
	for(int i = 0; i < accounts.count(); ++i) {
		QTreeWidgetItem *item = new QTreeWidgetItem;
		item->setText(ACCOUNTTREE_NAME, accounts[i]->name);
		item->setText(ACCOUNTTREE_DESC, accounts[i]->description);
		item->setText(ACCOUNTTREE_MONEY, Household::stringFromValueInt(accounts[i]->money));
		item->setText(ACCOUNTTREE_ID, QString::number(accounts[i]->id));
		item->setTextAlignment(ACCOUNTTREE_MONEY, Qt::AlignRight);
		if(accounts[i]->isDefault == ACCOUNT_DEFAULT) {
			QFont font = item->font(0);
			font.setBold(true);
			item->setFont(ACCOUNTTREE_NAME, font);
			item->setFont(ACCOUNTTREE_DESC, font);
			item->setFont(ACCOUNTTREE_MONEY, font);
			//item->setFont(ACCOUNTTREE_ID, font);
		}
		ui->treeWidget_accounts->addTopLevelItem(item);
	}
	ui->treeWidget_accounts->resizeColumnToContents(0);
}

void AccountsForm::onAddAccountClicked() {
	qDebug() << "AccountsForm::onAddAccountClicked()";
	AddEditAccountsDialog dlg(this);
	dlg.setShowHelpText(ui->label_helpText->isVisible());
	if(dlg.exec() == QDialog::Accepted) {
		Account a;
		a.name = dlg.getName();
		a.description = dlg.getDescription();
		a.money = 0;
		a.isDefault = ACCOUNT_NORMAL;
		if(_household->addAccount(a)) {
			refreshAccountList();
		}
	}
}

void AccountsForm::showContextMenu(QPoint p) {
	qDebug() << "AccountsForm::showContextMenu()";
	QMenu menu;
	QTreeWidgetItem *i = ui->treeWidget_accounts->itemAt(p);
	if(i != 0) {
		if(!_household->isAccountDefault(i->text(ACCOUNTTREE_ID).toInt())) {
			menu.addAction(tr("Set as default account"), this, SLOT(setDefaultAccount()));
		}
		menu.addAction(tr("Edit account"), this, SLOT(editAccount()));
		menu.addSeparator();
		menu.addAction(tr("Remove account"), this, SLOT(removeAccount()));
	}
	menu.exec(ui->treeWidget_accounts->viewport()->mapToGlobal(p));
}

void AccountsForm::editAccount() {
	qDebug() << "AccountsForm::editAccount()";
	QList<QTreeWidgetItem*> items = ui->treeWidget_accounts->selectedItems();
	if(items.count() == 1) {
		int aid = items[0]->text(ACCOUNTTREE_ID).toInt();
		AddEditAccountsDialog dlg(this);
		Account a;
		a.id = aid;
		a.name = _household->getAccountName(aid);
		a.description = _household->getAccountDescription(aid);
		a.money = _household->getAccountMoney(aid);
		a.uid = _household->getAccountUserId(aid);
		a.isDefault = _household->isAccountDefault(aid);
		dlg.setName(a.name);
		dlg.setDescription(a.description);
		dlg.setShowHelpText(ui->label_helpText->isVisible());
		if(dlg.exec() == QDialog::Accepted) {
			a.name = dlg.getName();
			a.description = dlg.getDescription();
			if(_household->editAccount(a)) {
				refreshAccountList();
			}
		}
	}
}

void AccountsForm::removeAccount() {
	qDebug() << "AccountsForm::removeAccount()";
	if(QMessageBox::question(this, tr("Question"), tr("Do you really want to remove this account?"),
							 QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes) {
		QList<QTreeWidgetItem*> items = ui->treeWidget_accounts->selectedItems();
		if(items.count() == 1) {
			int aid = items[0]->text(ACCOUNTTREE_ID).toInt();
			if(_household->isAccountDefault(aid)) {
				QMessageBox::information(this, tr("Information"), tr("You cannot remove default account."));
				return;
			}
			if(_household->removeAccount(items[0]->text(ACCOUNTTREE_ID).toInt())) {
				refreshAccountList();
			}
		}
	}
}

void AccountsForm::setDefaultAccount() {
	qDebug() << "AccountsForm::setDefaultAccount()";
	QList<QTreeWidgetItem*> items = ui->treeWidget_accounts->selectedItems();
	if(items.count() == 1) {
		int aid = items[0]->text(ACCOUNTTREE_ID).toInt();
		Account a;
		a.id = aid;
		a.name = _household->getAccountName(aid);
		a.description = _household->getAccountDescription(aid);
		a.money = _household->getAccountMoney(aid);
		a.uid = _household->getAccountUserId(aid);
		a.isDefault = ACCOUNT_DEFAULT;
		if(_household->setDefaultAccount(a)) {
			refreshAccountList();
		}
	}
}
