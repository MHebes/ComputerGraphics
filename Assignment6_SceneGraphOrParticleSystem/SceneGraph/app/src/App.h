/**
 * The primary application code.
 */

#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include <string>

class App : public QMainWindow {
  Q_OBJECT

public:
  App(std::string file, QWidget* parent = 0);
  virtual ~App();

signals:

public slots:

private:
  void buildGui();
  std::string m_file;
};
