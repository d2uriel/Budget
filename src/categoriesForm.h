#ifndef CATEGORIESFORM_H
#define CATEGORIESFORM_H

#include <QDialog>
#include "household.h"

namespace Ui {
class CategoriesForm;
}

class CategoriesForm : public QDialog {
	Q_OBJECT
	
public:
	explicit CategoriesForm(Household *household, QWidget *parent = 0);
	~CategoriesForm();
	
private:
	// Members.
	Ui::CategoriesForm	*ui;
	Household			*_household;

	// Methods.
	void				updateCategoriesTree();
	void				clearTreeWidgets();
	void				showContextMenu(const QPoint& p, const int type);
	void				addCategory(const int type);
	void				removeCategory(const int type);
	void				editCategory(const int type);

private slots:
	void				addExpenseCategoryClicked();
	void				addIncomeCategoryClicked();
	void				removeIncomeCategoryClicked();
	void				removeExpensesCategoryClicked();
	void				editIncomeCategoryClicked();
	void				editExpensesCategoryClicked();

	void				showContextMenuExpenses(QPoint p);
	void				showContextMenuIncome(QPoint p);
};

#endif // CATEGORIESFORM_H
