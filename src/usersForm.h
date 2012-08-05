#ifndef USERSFORM_H
#define USERSFORM_H

#include <QDialog>
#include "household.h"

namespace Ui {
class usersForm;
}

class UsersForm : public QDialog {
	Q_OBJECT
	
public:
	explicit UsersForm(Household *h, QWidget *parent = 0);
	~UsersForm();

	void setShowHelpText(bool show = true);

	void refreshUserList();
	
private:
	Ui::usersForm *ui;
	Household *_household;
	void clear();

private slots:
	void onAddUserClicked();
	void showContextMenu(QPoint p);
	void editUser();
	void removeUser();
	void changePassword();
};

#endif // USERSFORM_H
