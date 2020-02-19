/**
 * The primary application code.
 */

#include <QtCore>
#include <QtGui>
#include <QtWidgets>

class Application : public QMainWindow {
  Q_OBJECT

public:
  Application(QWidget* parent = 0);
  virtual ~Application();

signals:

public slots:

private:
  void buildGui();
};
