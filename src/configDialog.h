#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QDialog>
#include <QList>
#include <QString>

namespace Ui {
class ConfigDialog;
}

class ConfigDialog : public QDialog {
	Q_OBJECT
	
public:
	// Methods.
	explicit ConfigDialog(QWidget *parent = 0);
	~ConfigDialog();

	void		setAvailableLanguagePacks(const QList<QString>& languagePacks);
	QString		getLanguage();
	void		setLanguage(const QString& language);
	
private:
	Ui::ConfigDialog *ui;
};

#endif // CONFIGDIALOG_H
