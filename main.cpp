#include <QApplication>
#include <QtCore/QUrl>
#include <QtWebEngineWidgets/QWebEngineView>
#include <QDir>
#include <mutex>
#include <condition_variable>
#include <QtGui/QDesktopServices>
#include <QShowEvent>
#include <QtCore/QTimer>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonArray>
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QPrintDialog>
#include "poppler/qt5/poppler-qt5.h"
#include <QPrintPreviewDialog>
#include <iostream>
#include <memory>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  bool showPreview = argc > 2 && std::string(argv[1]) == "--preview"; 
  size_t startPdf = showPreview ? 2 : 1;

  std::vector<std::shared_ptr<Poppler::Document>> docs;
  for(size_t i = startPdf;i < argc;i++)
	  docs.emplace_back(Poppler::Document::load(argv[i]));

  auto printFunction = [&](QPrinter* printer) {
	  printer->setResolution(300);
	  printer->setFullPage(true);
	  auto resolution = printer->resolution();
	  QPainter painter;
	  if (!painter.begin(printer)) { // failed to open file
		  qWarning("failed to open file, is it writable?");
		  return;
	  }
	  bool isFirst = true;
	  for (auto& doc : docs) {
		  for (size_t i = 0;i < 1;i++) {
			  if (!isFirst)
				  printer->newPage();
			  isFirst = false;

			  std::unique_ptr<Poppler::Page> page(doc->page(i));
			  auto mult = 1.0;
			  auto image = page->renderToImage(resolution * mult, resolution * mult);
			  if (!image.isNull()) {
				  painter.drawImage(QRectF(0, 0, image.width() / mult, image.height() / mult), image,
					  QRectF(0, 0, image.width(), image.height()),
					  Qt::AutoColor | Qt::DiffuseAlphaDither | Qt::DiffuseDither | Qt::PreferDither
				  );
			  }
		  }
	  }
  };

  if (showPreview) {
	  QPrintPreviewDialog *pd = new QPrintPreviewDialog();

	  pd->connect(pd, &QPrintPreviewDialog::paintRequested, printFunction);
	  pd->setAttribute(Qt::WA_DeleteOnClose);
	  pd->show();
	  
	  return app.exec();
  }
  else {
	  QPrinter printer(QPrinter::HighResolution); 
	  
	  QPrintDialog* pd = new QPrintDialog(&printer);	  
	  if (pd->exec() == QDialog::Accepted) {
		  printFunction(&printer);
	  }	  
  }

  return 0;
}
