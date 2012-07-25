#include "usersForm.h"
#include "ui_usersForm.h"
#include "addEditUserDialog.h"
#include "changePasswordDialog.h"
#include <QDebug>
#include <QList>
#include <QString>
#include <QMessageBox>
#include <QMenu>
#include <QCryptographicHash>

UsersForm::UsersForm(Household *h, QWidget *parent) : QDialog(parent), ui(new Ui::usersForm) {
	qDebug() << "UsersForm::UsersForm()";
	ui->setupUi(this);
	_household = h;
	ui->treeWidget_users->setColumnHidden(USERSTREE_ID, true);
	ui->treeWidget_users->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(ui->treeWidget_users, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
	connect(ui->pushButton_addUser, SIGNAL(clicked()), this, SLOT(onAddUserClicked()));
	refreshUserList();
}

UsersForm::~UsersForm() {
	qDebug() << "UsersForm::~UsersForm()";
	clear();
	delete ui;
}

void UsersForm::clear() {
	qDebug() << "UsersForm::clear()";
	while(ui->treeWidget_users->topLevelItemCount() > 0) {
		delete ui->treeWidget_users->takeTopLevelItem(0);
	}
}

void UsersForm::refreshUserList() {
	qDebug() << "UsersForm::refreshUserList()";
	clear();
	QList<User*> users = _household->getUserList();
	for(int i = 0; i < users.count(); ++i) {
		QTreeWidgetItem *item = new QTreeWidgetItem;
		item->setText(USERSTREE_NAME, users[i]->name);
		item->setText(USERSTREE_DESC, users[i]->description);
		if(users[i]->level == ACCESS_ADMIN) {
			item->setText(USERSTREE_LEVEL, tr("Administrator"));
		} else {
			item->setText(USERSTREE_LEVEL, tr("User"));
		}
		item->setText(USERSTREE_ID, QString::number(users[i]->id));
		ui->treeWidget_users->addTopLevelItem(item);
	}
}

void UsersForm::onAddUserClicked() {
	qDebug() << "UsersForm::onAddUserClicked()";
	AddEditUserDialog dlg(this);
	if(dlg.exec() == QDialog::Accepted) {
		User u;
		u.name = dlg.getName();
		u.password = dlg.getPassword();
		u.description = dlg.getDescription();
		if(dlg.getAccessLevel() == tr("Administrator")) {
			u.level = ACCESS_ADMIN;
		} else {
			u.level = ACCESS_USER;
		}
		_household->addUser(u);
		refreshUserList();
	}
}

void UsersForm::showContextMenu(QPoint p) {
	qDebug() << "UsersForm::showContextMenu()";
	QMenu menu;
	QTreeWidgetItem *i = ui->treeWidget_users->itemAt(p);
	if(i != 0) {
		menu.addAction(tr("Edit user"), this, SLOT(editUser()));
		menu.addAction(tr("Change password"), this, SLOT(changePassword()));
		menu.addSeparator();
		menu.addAction(tr("Remove user"), this, SLOT(removeUser()));
	}
	menu.exec(ui->treeWidget_users->viewport()->mapToGlobal(p));
}

void UsersForm::editUser() {
	qDebug() << "UsersForm::editUser()";
	QList<QTreeWidgetItem*> items = ui->treeWidget_users->selectedItems();
	if(items.count() == 1) {
		int uid = items[0]->text(USERSTREE_ID).toInt();
		User u;
		u.name = _household->getUserName(uid);
		u.id = uid;
		u.level = _household->getUserAccessLevel(uid);
		u.password = _household->getUserPassword(uid);
		AddEditUserDialog dlg(this);
		dlg.setName(u.name);
		if(u.level == ACCESS_ADMIN) {
			dlg.setAccessLevel(tr("Administrator"));
		} else {
			dlg.setAccessLevel(tr("User"));
		}
		dlg.setDescription(u.description);
		dlg.setPasswordsDisabled();
		if(_household->isUserLastAdmin(u.id)) {
			dlg.setAccessLevelDisabled();
		}
		if(dlg.exec() == QDialog::Accepted) {
			u.name = dlg.getName();
			//u->password = dlg->getPassword();
			u.description = dlg.getDescription();
			if(dlg.getAccessLevel() == tr("Administrator")) {
				u.level = ACCESS_ADMIN;
			} else {
				u.level = ACCESS_USER;
			}
			if(_household->editUser(u)) {
				refreshUserList();
			}
		}
	}
}

void UsersForm::removeUser() {
	qDebug() << "UsersForm::removeUser()";
	if(QMessageBox::question(this, tr("Question"), tr("Do you really want to remove this user?"),
							 QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes) {
		QList<QTreeWidgetItem*> items = ui->treeWidget_users->selectedItems();
		if(items.count() == 1) {
			if(_household->removeUser(items[0]->text(USERSTREE_ID).toInt())) {
				refreshUserList();
			}
		}
	}
}

void UsersForm::changePassword() {
	qDebug() << "UsersForm::changePassword()";
	ChangePasswordDialog dlg(this);
	if(dlg.exec() == QDialog::Accepted) {
		QList<QTreeWidgetItem*> items = ui->treeWidget_users->selectedItems();
		if(items.count() == 1) {
			if(_household->verifyPassword(items[0]->text(USERSTREE_NAME), dlg.getCurrentPassword()) != -1) {
				User user;
				user.id = items[0]->text(USERSTREE_ID).toInt();
				user.level = _household->getUserAccessLevel(user.id);
				user.name = items[0]->text(USERSTREE_NAME);
				QCryptographicHash pwdHash(QCryptographicHash::Md5);
				pwdHash.addData(dlg.getNewPassword().toUtf8());
				user.password = pwdHash.result();
				_household->changeUserPassword(user);
			}
		}
	}
}
