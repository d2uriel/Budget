#ifndef ADDCATEGORYDIALOG_H
#define ADDCATEGORYDIALOG_H

#include <QDialog>
#include <QList>
#include <QString>
#include "household.h"

namespace Ui {
class AddCategoryDialog;
}

class AddCategoryDialog : public QDialog {
	Q_OBJECT
	
public:
	// Members.

	// Methods.
	explicit AddCategoryDialog(QWidget *parent = 0);
	~AddCategoryDialog();

	void	setShowHelpText(bool show = true);

	QString getCategoryName();
	void	setCategoryName(const QString& name);

	QString getParentName();
	void	setCurrentParent(const QString& p);

	void fillCategoriesComboBox(const QList<QString> cn);

	void setDialogTitle(const QString& title);
	
private:
	// Members.
	Ui::AddCategoryDialog	*ui;
	QList<QString>			_categoryNames;

	// Methods.

private slots:
	void verifyDialog();
};

#endif // ADDCATEGORYDIALOG_H
