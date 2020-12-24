#include "imagepopup.h"

#include <DLog>
#include <QApplication>
#include <QDesktopWidget>
#include <QShortcut>
#include <QImageReader>
#include <QScreen>

LDA_BEGIN_NAMESPACE

const int kBorderSize = 12;
const int kCloseBtnSize = 48;

ImagePopup::ImagePopup(QWidget *parent)
    : QDialog(parent)
{
    this->setObjectName("ImagePopup");
    this->init();

    connect(close, &Dtk::Widget::DIconButton::clicked, this, &QDialog::close);
}

ImagePopup::~ImagePopup()
{
    close->~DDialogCloseButton();
    img->~DLabel();
}

void ImagePopup::open(QImage &im)
{
    const QRect screen_rect = QGuiApplication::screenAt(this->pos())->geometry();
    const int pixmap_max_width = static_cast<int>(screen_rect.width() * 0.8);
    const int pixmap_max_height = static_cast<int>(screen_rect.height() * 0.8);

    if ((im.width() > pixmap_max_width) || (im.height() > pixmap_max_height)) {
        im = im.scaled(pixmap_max_width, pixmap_max_height, Qt::KeepAspectRatio,
                             Qt::SmoothTransformation);
    }
    if (im.width() < 50 && im.height() < 50) {
        return;
    }
    this->move(screen_rect.topLeft());
    this->resize(screen_rect.size());
    this->showFullScreen();

    img->setPixmap(QPixmap::fromImage(im));
    img->setFixedSize(im.width(), im.height());

    QRect img_rect = img->rect();
    img_rect.moveTo(static_cast<int>((screen_rect.width() - im.width()) / 2.0),
                    static_cast<int>((screen_rect.height() - im.height()) / 2.0));
    img->move(img_rect.topLeft());

    const QPoint top_right_point = img_rect.topRight();
    close->move(top_right_point.x() - kCloseBtnSize / 2,
                        top_right_point.y() - kCloseBtnSize / 2);
    close->show();
    close->raise();
}

void ImagePopup::init()
{
    img = new DLabel(this);
    img->setObjectName("ImageLabel");

    close = new DDialogCloseButton(this);
    close->setFlat(true);
    close->setObjectName("CloseButton");
    close->raise();
    close->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    close->setIconSize(QSize(36, 36));
    close->setFixedSize(45, 45);

    this->setContentsMargins(kBorderSize, kBorderSize, kBorderSize, kBorderSize);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::BypassWindowManagerHint
                         | Qt::Dialog | Qt::WindowStaysOnTopHint);
    this->setAttribute(Qt::WA_TranslucentBackground, true);
    this->setModal(true);

    QShortcut *scHideDialog = new QShortcut(QKeySequence(Qt::Key_Escape), this);
    scHideDialog->setContext(Qt::ApplicationShortcut);
    scHideDialog->setAutoRepeat(false);

    connect(scHideDialog, &QShortcut::activated, this, [this] {
        qDebug() << "pressed esc!" << endl;
        this->hide();
    });
}

void ImagePopup::mousePressEvent(QMouseEvent *event)
{
    QWidget::mousePressEvent(event);
    this->hide();
}

void ImagePopup::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.fillRect(0, 0, this->width(), this->height(), QColor(0, 0, 0, 77));
}

LDA_END_NAMESPACE
