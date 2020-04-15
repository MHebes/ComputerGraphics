#include "App.h"

#include "BasicWidget.h"

App::App(std::string objfile, QWidget* parent)
    : m_objfile(objfile), QMainWindow(parent)
{
  buildGui();
}

App::~App() {}

void App::buildGui()
{
  // A simple menubar.
  QMenuBar* menu = menuBar();
  QMenu* file = menu->addMenu("File");
  QAction* exit = file->addAction("Quit", [this]() { close(); });

  // Our basic widget.
  BasicWidget* widget = new BasicWidget(m_objfile, this);
  setCentralWidget(widget);
}
