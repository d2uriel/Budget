#include "categoriesForm.h"
#include "ui_categoriesForm.h"
#include "constants.h"
#include <QtSql/QSqlQuery>
#include <QTreeWidgetItem>
#include <QFileDialog>
#include <QList>
#include "addCategoryDialog.h"
#include <QMessageBox>
#include <QMenu>
#include <QAction>
#include <QDebug>
#include <QTextStream>

CategoriesForm::CategoriesForm(Household *household, QWidget *parent) : QDialog(parent), ui(new Ui::CategoriesForm), _household(household) {
	qDebug() << "CategoriesForm::CategoriesForm()";
	ui->setupUi(this);
	ui->treeWidget_expensesCategories->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->treeWidget_incomeCategories->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(ui->treeWidget_expensesCategories, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenuExpenses(QPoint)));
	connect(ui->treeWidget_incomeCategories, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenuIncome(QPoint)));
	connect(ui->pushButton_addExpensesCategory, SIGNAL(clicked()), this, SLOT(addExpenseCategoryClicked()));
	connect(ui->pushButton_addIncomeCategory, SIGNAL(clicked()), this, SLOT(addIncomeCategoryClicked()));
	connect(ui->pushButton_OK, SIGNAL(clicked()), this, SLOT(accept()));
	updateCategoriesTree();
}

CategoriesForm::~CategoriesForm() {
	qDebug() << "CategoriesForm::~CategoriesForm()";
	clearTreeWidgets();
	delete ui;
}

void CategoriesForm::clearTreeWidgets() {
	qDebug() << "CategoriesForm::clearTreeWidgets()";
	// Clear tree widgets.
	QTreeWidgetItem *t = ui->treeWidget_incomeCategories->takeTopLevelItem(0);
	while(t != 0) {
		delete t;
		t = ui->treeWidget_incomeCategories->takeTopLevelItem(0);
	}
	t = ui->treeWidget_expensesCategories->takeTopLevelItem(0);
	while(t != 0) {
		delete t;
		t = ui->treeWidget_expensesCategories->takeTopLevelItem(0);
	}
}

void CategoriesForm::updateCategoriesTree() {
	qDebug() << "CategoriesForm::updateCategoriesTree()";
	clearTreeWidgets();

	const QList<Category*> parentCategories = _household->getParentCategories();
	for(int i = 0; i < parentCategories.count(); ++i) {
		Category *c = parentCategories[i];
		QTreeWidgetItem *i = new QTreeWidgetItem();
		i->setText(0, c->name);
		i->setText(1, _household->getUserName(c->uid));
		if(c->type == TYPE_INCOME) {
			ui->treeWidget_incomeCategories->addTopLevelItem(i);
		} else if(c->type == TYPE_EXPENSE) {
			ui->treeWidget_expensesCategories->addTopLevelItem(i);
		}
	}

	const QList<Category*> childCategories = _household->getChildCategories();
	for(int i = 0; i < childCategories.count(); ++i) {
		Category *c = childCategories[i];
		QTreeWidgetItem *i = new QTreeWidgetItem();
		i->setText(0, c->name);
		i->setText(1, _household->getUserName(c->uid));
		if(c->type == TYPE_INCOME) {
			QList<QTreeWidgetItem*> items = ui->treeWidget_incomeCategories->findItems(_household->getCategoryName(c->pid), Qt::MatchExactly);
			items[0]->addChild(i);
		} else if(c->type == TYPE_EXPENSE) {
			QList<QTreeWidgetItem*> items = ui->treeWidget_expensesCategories->findItems(_household->getCategoryName(c->pid), Qt::MatchExactly);
			items[0]->addChild(i);
		}
	}
	/*
	const QList<Category*>* categories = _household->getCategories();
	// id = 0, pid = 1, name = 2, type = 3
	for(int i = 0; i < categories->count(); ++i) {
		Category *c = categories->at(i);
		// First add parent categories.
		if(c->pid != -1) {
			continue;
		}
		QTreeWidgetItem *i = new QTreeWidgetItem();
		i->setText(0, c->name);
		i->setText(1, _household->getUserName(c->uid));
		if(c->type == TYPE_INCOME) {
			// No parent = top level item.
			if(c->pid == -1) {
				ui->treeWidget_incomeCategories->addTopLevelItem(i);
			} else {
				QList<QTreeWidgetItem*> items = ui->treeWidget_incomeCategories->findItems(_household->getCategoryName(c->pid), Qt::MatchExactly);
				items[0]->addChild(i);
			}

		} else if(c->type == TYPE_EXPENSE) {
			// No parent = top level item.
			if(c->pid == -1) {
				ui->treeWidget_expensesCategories->addTopLevelItem(i);
			} else {
				QList<QTreeWidgetItem*> items = ui->treeWidget_expensesCategories->findItems(_household->getCategoryName(c->pid), Qt::MatchExactly);
				items[0]->addChild(i);
			}
		} else {

		}
	}
	*/
	ui->treeWidget_incomeCategories->expandAll();
	ui->treeWidget_expensesCategories->expandAll();
	ui->treeWidget_incomeCategories->setColumnWidth(0, 250);
	ui->treeWidget_expensesCategories->setColumnWidth(0, 250);
}

void CategoriesForm::addCategory(const int type) {
	qDebug() << "CategoriesForm::addCategory()";
	AddCategoryDialog dlg;
	QTreeWidget *tree = 0;
	if(type == TYPE_INCOME) {
		tree = ui->treeWidget_incomeCategories;
		dlg.setDialogTitle(tr("Add income category"));
	} else if(type == TYPE_EXPENSE) {
		tree = ui->treeWidget_expensesCategories;
		dlg.setDialogTitle(tr("Add expenses category"));
	}
	dlg.fillCategoriesComboBox(_household->getCategoryNames(type));
	if(tree->selectedItems().count() == 1) {
		QList<QTreeWidgetItem*> items = tree->selectedItems();
		dlg.setCurrentParent(items[0]->text(0));
	} else {

	}
	if(dlg.exec() == QDialog::Accepted) {
		Category c;
		c.name = dlg.getCategoryName();
		c.pid = _household->getCategoryId(dlg.getParentName().trimmed());
		c.type = type;
		_household->addCategory(c);
		updateCategoriesTree();
	}
}

void CategoriesForm::addExpenseCategoryClicked() {
	qDebug() << "CategoriesForm::addExpenseCategoryClicked()";
	addCategory(TYPE_EXPENSE);
}

void CategoriesForm::addIncomeCategoryClicked() {
	qDebug() << "CategoriesForm::addIncomeCategoryClicked()";
	addCategory(TYPE_INCOME);
}

void CategoriesForm::removeCategory(const int type) {
	qDebug() << "CategoriesForm::removeCategory()";
	if(QMessageBox::question(this, tr("Question"), tr("Are you really sure you want to remove this category?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No) \
		== QMessageBox::No) {
		return;
	}
	bool accept = true;
	QTreeWidget *tree = 0;
	if(type == TYPE_INCOME) {
		tree = ui->treeWidget_incomeCategories;
	} else if(type == TYPE_EXPENSE) {
		tree = ui->treeWidget_expensesCategories;
	}
	QList<QTreeWidgetItem*> items = tree->selectedItems();
	for(int i = 0; i < items.count(); ++i) {
		if(items[i]->childCount() > 0) {
			int retVal = QMessageBox::question(this, tr("Question"), tr("This will remove all child categories.\nDo you want to continue?"),
												QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
			if(retVal == QMessageBox::No) {
				accept = false;
			}
		}
		if(accept) {
			_household->removeCategory(items[i]->text(0));
			updateCategoriesTree();
		}
	}
}

void CategoriesForm::removeIncomeCategoryClicked() {
	qDebug() << "CategoriesForm::removeIncomeCategory()";
	removeCategory(TYPE_INCOME);
}

void CategoriesForm::removeExpensesCategoryClicked() {
	qDebug() << "CategoriesForm::removeExpensesCategory()";
	removeCategory(TYPE_EXPENSE);
}

void CategoriesForm::editCategory(const int type) {
	qDebug() << "CategoriesForm::editCategory()";
	AddCategoryDialog dlg;
	QTreeWidget *tree = 0;
	if(type == TYPE_INCOME) {
		tree = ui->treeWidget_incomeCategories;
		dlg.setDialogTitle(tr("Edit income category"));
	} else if(type == TYPE_EXPENSE) {
		tree = ui->treeWidget_expensesCategories;
		dlg.setDialogTitle(tr("Edit expenses category"));
	}
	dlg.fillCategoriesComboBox(_household->getCategoryNames(type));
	QList<QTreeWidgetItem*> items;
	if(tree->selectedItems().count() == 1) {
		items = tree->selectedItems();
		dlg.setCurrentParent(_household->getParentName(items[0]->text(0)));
		dlg.setCategoryName(items[0]->text(0));
	} else {
		return;
	}
	if(dlg.exec() == QDialog::Accepted) {
		Category c;
		c.name = dlg.getCategoryName();
		c.pid = _household->getCategoryId(dlg.getParentName().trimmed());
		c.id = _household->getCategoryId(items[0]->text(0).trimmed());
		c.type = type;
		if(_household->editCategory(c, c.id)) {
			updateCategoriesTree();
		}
	}
}

void CategoriesForm::editIncomeCategoryClicked() {
	qDebug() << "CategoriesForm::editIncomeCategoryClicked()";
	editCategory(TYPE_INCOME);
}

void CategoriesForm::editExpensesCategoryClicked() {
	qDebug() << "CategoriesForm::editExpensesCategoryClicked()";
	editCategory(TYPE_EXPENSE);
}

void CategoriesForm::showContextMenuExpenses(QPoint p) {
	qDebug() << "CategoriesForm::showContextMenuExpenses()";
	showContextMenu(p, TYPE_EXPENSE);
}

void CategoriesForm::showContextMenuIncome(QPoint p) {
	qDebug() << "CategoriesForm::showContextMenuIncome()";
	showContextMenu(p, TYPE_INCOME);
}

void CategoriesForm::showContextMenu(const QPoint &p, const int type) {
	qDebug() << "CategoriesForm::showContextMenu()";
	QMenu *menu = new QMenu;
	QTreeWidget *tree = 0;
	QAction *add = new QAction(tr("Add child category"), menu);
	QAction *remove = new QAction(tr("Remove category"), menu);
	QAction *edit = new QAction(tr("Edit category"), menu);
	if(type == TYPE_INCOME) {
		tree = ui->treeWidget_incomeCategories;
		connect(add, SIGNAL(triggered()), this, SLOT(addIncomeCategoryClicked()));
		connect(remove, SIGNAL(triggered()), this, SLOT(removeIncomeCategoryClicked()));
		connect(edit, SIGNAL(triggered()), this, SLOT(editIncomeCategoryClicked()));
	} else if(type == TYPE_EXPENSE) {
		tree = ui->treeWidget_expensesCategories;
		connect(add, SIGNAL(triggered()), this, SLOT(addExpenseCategoryClicked()));
		connect(remove, SIGNAL(triggered()), this, SLOT(removeExpensesCategoryClicked()));
		connect(edit, SIGNAL(triggered()), this, SLOT(editExpensesCategoryClicked()));
	}
	QTreeWidgetItem *i = tree->itemAt(p);
	if(i != 0) {
		if(i->parent() == 0) {
			menu->addAction(add);
		}
		if(_household->getUserId(tree->itemAt(p)->text(1)) == _household->currentUserId()
				|| _household->currentUserAccessLevel() == ACCESS_ADMIN) {
			menu->addAction(edit);
			menu->addSeparator();
			menu->addAction(remove);
		}
	}
	menu->exec(tree->viewport()->mapToGlobal(p));
	menu->removeAction(add);
	menu->removeAction(edit);
	menu->removeAction(remove);
	delete add;
	delete edit;
	delete remove;
	delete menu;
}
