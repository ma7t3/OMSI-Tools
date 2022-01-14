#include "wfonts.h"
#include "ui_wfonts.h"


wFonts::wFonts() :
    QMainWindow(),
    ui(new Ui::wFonts)
{
    qInfo().noquote() << "Starting " + moduleName + "...";

    qDebug() << "Set up UI...";
    ui->setupUi(this);
    adjustSize();
    qDebug() << "UI set";

    setWindowTitle(OTName + " - " + tr("fonts"));

    loadRecentFiles();

    // Set timer for autosave
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(autosave()));
    int autosaveDuration = set.read("main", "autosaveDuration").toInt();
    if (autosaveDuration == 0)
        autosaveDuration = 15;
    timer->start(autosaveDuration * 1000);

    misc.checkBackupFolderExistance();

    // Set Comboboxes
    ui->cobxEncoding->addItem("ANSI");
    ui->cobxEncoding->addItem("UTF-8");

    // Set only-Numbers
    ui->ledHighestPixelInFontRow->setValidator(new QIntValidator(0, 10000, this));
    ui->ledLeftPixel->setValidator(new QIntValidator(0, 10000, this));
    ui->ledRightPixel->setValidator(new QIntValidator(0, 10000, this));
    ui->sbxMaxHeigthOfChars->clear();
    ui->sbxDistanceBetweenChars->clear();

    // and for Debug-Action
    ui->actionGoToNextError->setEnabled(false);

    ui->btnNextResult->setEnabled(false);
    ui->btnFind->setEnabled(false);

    enableFontArea(false);

    strListChars = new QStringListModel();

    ui->twgFont->setCurrentIndex(0);
    ui->gbxSearchChar->setVisible(false);

    // Load settings
    setStyleSheet(set.read("main", "theme").toString());

    setUnsaved(false);
    ui->lvwChars->setModel(strListChars);
    connect(ui->lvwChars->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &wFonts::charSelectionChanged);
    reloadValidProperty();

    qInfo().noquote() << moduleName + " started successfully.";
}

wFonts::~wFonts()
{
    qInfo().noquote() << moduleName + " is closing...";
    delete ui;
    delete strListChars;
}

void wFonts::closeEvent (QCloseEvent *event)
{
    if (unsaved)
    {
        int msgResult = msg.unsavedContent(this);
        if (msgResult == -1)
        {
            event->ignore();
            return;
        }
        else if (msgResult == 1)
            save(OTFileMethods::save, font.path);
    }
}

void wFonts::dragEnterEvent(QDragEnterEvent *e)
{
    if (e->mimeData()->hasUrls())
        e->acceptProposedAction();
}

void wFonts::dropEvent(QDropEvent *e)
{
    QString fileName = e->mimeData()->urls().at(0).toLocalFile();
    qDebug().noquote() << "Dropped file:" << fileName;

    if (QFileInfo(fileName).suffix() != "oft")
    {
        QMessageBox::critical(this, tr("Invalid format", "Note #1"), tr("The dropped file is no font file (*.oft)!"));
        qWarning() << "Dropped file is no OFT file!";
        return;
    }

    QMessageBox::StandardButton reply = QMessageBox::question(this, tr("Open font file", "Note #1"), tr("Open drag and drop font file now?\n%1").arg(fileName));

    if (reply == QMessageBox::Yes)
        open(OTFileMethods::open, fileName);
}

/// \brief Deletes current char
void wFonts::on_btnDeleteSelection_clicked()
{
    wFonts::on_actionDeleteSelection_triggered();
}

/// \brief Creates a new char
void wFonts::on_btnNewChar_clicked()
{
    on_actionNewChar_triggered();
}

/// \brief Moves current char up
void wFonts::on_btnMoveUp_clicked()
{
    on_actionMoveCharUp_triggered();
}

/// \brief Moves current char down
void wFonts::on_btnMoveDown_clicked()
{
    on_actionMoveCharDown_triggered();
}

/// \brief Opens a font
void wFonts::on_actionOpen_triggered()
{
    open(OTFileMethods::open);
}

/// \brief Reloads a font
void wFonts::on_actionReload_triggered()
{
    if (font.path != "")
        open(OTFileMethods::reOpen, font.path);
}

/// \brief Saves a font
void wFonts::on_actionSave_triggered()
{
    save(OTFileMethods::save, font.path);
}

/// \brief Saves a font in a specific path
void wFonts::on_actionSaveAs_triggered()
{
    save(OTFileMethods::saveAs, font.path);
}

/// \brief Enables or diables the whole window
void wFonts::enableView(bool status)
{
    centralWidget()->setEnabled(status);
}

/// \brief Loads recent files from settings
void wFonts::loadRecentFiles()
{
    qDebug() << "Read recent files...";
    QStringList recentFiles = set.read(moduleName, "recentFiles").toStringList();

    if (recentFiles.isEmpty())
    {
        ui->menuRecentlyOpenedFonts->setEnabled(false);
        return;
    }
    else
    {
        ui->menuRecentlyOpenedFonts->setEnabled(true);
        ui->menuRecentlyOpenedFonts->clear();
    }

    int i = 1;
    foreach (QString current, recentFiles)
    {
        QAction *action = new QAction();

        action->setData(current);
        action->setText(QString::number(i) + "\t" + QFile(current).fileName());
        action->setVisible(true);
        switch (i)
        {
            case 1: action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_1)); break;
            case 2: action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_2)); break;
            case 3: action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_3)); break;
            case 4: action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_4)); break;
            case 5: action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_5)); break;
            case 6: action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_6)); break;
            case 7: action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_7)); break;
            case 8: action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_8)); break;
            case 9: action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_9)); break;
            case 10: action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_0)); break;
        }

        connect(action, &QAction::triggered, this, [=]() { this->open(OTFileMethods::open, action->data().toString()); });

        ui->menuRecentlyOpenedFonts->addAction(action);
        i++;
    }
}

/// \brief Saves recent files to settings
void wFonts::saveRecentFiles(QString absoluteNewFilePath)
{
    qDebug() << "Save recent files...";
    QStringList recentFiles = set.read(moduleName, "recentFiles").toStringList();

    if (recentFiles.contains(absoluteNewFilePath))
        recentFiles.removeAt(recentFiles.indexOf(absoluteNewFilePath));

    recentFiles.prepend(absoluteNewFilePath);

    while (recentFiles.count() > maxRecentFileCount)
        recentFiles.removeLast();

    set.write(moduleName, "recentFiles", recentFiles);

    loadRecentFiles();
}

/// \brief Sets unsaved state for current session
void wFonts::setUnsaved(bool state)
{
    if (state)
        unsaved = true;
    else
        unsaved = false;
}

/// \brief Check current char for errors
void wFonts::reloadValidProperty()
{
    ui->lblFontName->setStyleSheet("");
    ui->lblColorTexture->setStyleSheet("");
    ui->lblAlphaTexture->setStyleSheet("");
    ui->lblMaxHeigthOfChars->setStyleSheet("");
    ui->lblDistanceBetweenChars->setStyleSheet("");

    if (font.name == "")
        ui->lblFontName->setStyleSheet("color:red");

    if ((font.colorTexture != "") && !QFile(set.read("main", "mainDir").toString() + "/Fonts/" + font.colorTexture).exists())
        ui->lblColorTexture->setStyleSheet("color:red");

    if (font.alphaTexture == "" || !QFile(set.read("main", "mainDir").toString() + "/Fonts/" + font.alphaTexture).exists())
        ui->lblAlphaTexture->setStyleSheet("color:red");

    if (ui->sbxMaxHeigthOfChars->text() == "" || (font.maxHeightOfChars == 0))
        ui->lblMaxHeigthOfChars->setStyleSheet("color:red");

    if (ui->sbxDistanceBetweenChars->text() == "")
        ui->lblDistanceBetweenChars->setStyleSheet("color:red");

    if (QFile(set.read("main", "mainDir").toString() + "/Fonts/" + font.alphaTexture).exists())
    {
        QImage alphaTexture(set.read("main", "mainDir").toString() + "/Fonts/" + font.alphaTexture);

        if (alphaTexture.width() != 0 || alphaTexture.height() != 0)
        {
            if (font.maxHeightOfChars > QString::number(alphaTexture.height()).toInt())
                ui->lblMaxHeigthOfChars->setStyleSheet("color:red");

            if (font.distanceBetweenChars > QString::number(alphaTexture.width()).toInt())
                ui->lblDistanceBetweenChars->setStyleSheet("color:red");
        }
    }
}

/// \brief Checks current char
void wFonts::checkCurrentChar()
{
    if (font.charList.count() == 0)
        return;

    ui->lblCharacter->setStyleSheet("");
    ui->lblRightPixel->setStyleSheet("");
    ui->lblLeftPixel->setStyleSheet("");
    ui->lblHighestPixelInFontRow->setStyleSheet("");

    OTCharacterModel character = font.charList.at(ui->lvwChars->currentIndex().row());

    if (character.character == "")
        ui->lblCharacter->setStyleSheet("color:red");
    else
    {
        QList<QString> tempCharList;
        foreach (OTCharacterModel current, font.charList)
            tempCharList << current.character;

        if (tempCharList.indexOf(character.character) != -1)
        {
            tempCharList.removeAt(tempCharList.indexOf(character.character));
            if (tempCharList.indexOf(character.character) != -1)
                ui->lblCharacter->setStyleSheet("color:red");
        }
    }

    if (character.leftPixel >= character.rightPixel)
    {
        ui->lblRightPixel->setStyleSheet("color:goldenrod");
        ui->lblLeftPixel->setStyleSheet("color:goldenrod");
    }

    if (ui->ledRightPixel->text() == "")
        ui->lblRightPixel->setStyleSheet("color:red");

    if (ui->ledLeftPixel->text() == "")
        ui->lblLeftPixel->setStyleSheet("color:red");

    if (ui->ledHighestPixelInFontRow->text() == "")
        ui->lblHighestPixelInFontRow->setStyleSheet("color:red");

    if (QFile(set.read("main", "mainDir").toString() + "/Fonts/" + font.alphaTexture).exists())
    {
        QImage alphaTexture(set.read("main", "mainDir").toString() + "/Fonts/" + font.alphaTexture);

        if (alphaTexture.width() != 0 || alphaTexture.height() != 0)
        {
            if (character.rightPixel > QString::number(alphaTexture.width()).toInt())
                ui->lblRightPixel->setStyleSheet("color:red");

            if (character.leftPixel > QString::number(alphaTexture.width()).toInt())
                ui->lblLeftPixel->setStyleSheet("color:red");

            if (character.highestPixelInFontRow > QString::number(alphaTexture.width()).toInt())
                ui->lblHighestPixelInFontRow->setStyleSheet("color:red");
        }
    }
}

/// \brief Saves the font automatically
void wFonts::autosave()
{
    if ((set.read("main", "autosave") == "true") && (font.charList.count() != 0))
    {
        qDebug() << "Autosave called";
        save(OTFileMethods::backupSave);
    }
}

/// \brief Clear the whole font
void wFonts::selectAllAndClear(bool onlyChar)
{
    if (!onlyChar)
    {
        setUnsaved();
        font.clear();
        strListChars->setStringList(QStringList());

        // Clear LineEdits
        ui->ledFontName->clear();
        ui->ledColorTexture->clear();
        ui->ledAlphaTexture->clear();
        ui->sbxMaxHeigthOfChars->clear();
        ui->sbxDistanceBetweenChars->clear();
    }

    ui->ledCharacter->clear();
    ui->ledLeftPixel->clear();
    ui->ledRightPixel->clear();
    ui->ledHighestPixelInFontRow->clear();
    ui->ledComment->clear();

    reloadCharList();

    enableFontArea(false);
}

/// \brief Moves cursor / chars up / down
void wFonts::move(int selection, QString action)
{
    if ((selection == 0 && action == "UP") || (selection > font.charList.count() - 1 && action == "DOWN"))
        return;

    int moving;

    if (action == "UP")
        moving = selection - 1;
    else if (action == "DOWN")
        moving = selection + 1;
    else
    {
        qWarning().noquote() << "Invalid move parameter: " + action;
        return;
    }
    setUnsaved();

    // Move the selected item down / up
    font.charList.move(selection, moving);
    reloadCharList();
    ui->lvwChars->setCurrentIndex(strListChars->index(moving));
}

/// \brief Enables or disables any objects for char editing
void wFonts::enableFontArea(bool status)
{
    ui->ledCharacter->setEnabled(status);
    ui->ledLeftPixel->setEnabled(status);
    ui->ledRightPixel->setEnabled(status);
    ui->ledHighestPixelInFontRow->setEnabled(status);
    ui->ledComment->setEnabled(status);
    ui->btnDeleteSelection->setEnabled(status);
    ui->btnMoveDown->setEnabled(status);
    ui->btnMoveUp->setEnabled(status);
    ui->actionMoveCharDown->setEnabled(status);
    ui->actionMoveCharUp->setEnabled(status);
    ui->actionDeleteSelection->setEnabled(status);
    ui->actionFindChar->setEnabled(status);
    ui->actionCopyChars->setEnabled(status);
    ui->actionGoToNextError->setEnabled(status);

    if (!status)
    {
        ui->lblCharacter->setStyleSheet("");
        ui->lblRightPixel->setStyleSheet("");
        ui->lblLeftPixel->setStyleSheet("");
        ui->lblHighestPixelInFontRow->setStyleSheet("");
    }
}

/// \brief Opens a font
void wFonts::open(OTFileMethods::fileMethods method, QString filen)
{
    if (unsaved)
    {
        int msgResult = msg.unsavedContent(this);
        if (msgResult == -1)
            return;
        else if (msgResult == 1)
            save(OTFileMethods::save, font.path);
    }

    qInfo() << "Open file...";
    qDebug().noquote() << "Open method:" << method;
    if (method == OTFileMethods::open)
    {
        if (filen == "")
        {
            qDebug() << "Open with file dialog";
            filen = QFileDialog::getOpenFileName(this, tr("Open font...", "Note #1"), set.read("main", "mainDir").toString() + "/Fonts", tr("OMSI font file") + " (*.oft)");
            if (filen == "")
                return;
            else
                selectAllAndClear();
        }
        font.path = filen;
    }

    // Make an direct (= more saver) autosave by coping the file
    if (method != OTFileMethods::silentOpen)
    {
        qDebug() << "Create font file backup...";
        QFile::copy(font.path, "backup/font_backup_" + misc.getDate("yyyyMMdd") + "_" + misc.getTime("hhmmss") + " " + font.name + "_afterOpen.oft");
    }

    // Cut out only the file name and put it into the window title
    if (method != OTFileMethods::silentOpen)
    {
        QFileInfo fileInfo(QFile(font.path).fileName());
        QString filenameWithoutPath(fileInfo.fileName());
        setTitle(filenameWithoutPath);
    }

    if (method == OTFileMethods::open)
        saveRecentFiles(QDir().absoluteFilePath(font.path));

    ui->twgFont->setCurrentIndex(1);

    OTFontModel tempFont = font;
    if (method == OTFileMethods::silentOpen)
        tempFont.path = filen;

    selectAllAndClear();
    tempFont = filehandler.openFont(tempFont.path, encoding);
    if (tempFont.error)
    {
        if (method != OTFileMethods::silentOpen)
            msg.errorWhileOpeningOmsi(this, tempFont.path);
        return;
    }
    else if (tempFont.moreThanOneFont)
    {
        if (method != OTFileMethods::silentOpen)
            QMessageBox::warning(this, tr("Open font"), tr("Attention: The selected font file contains more than one font. The application cannot read multiple fonts. Please split each font in this file into seperate files. If the font is saved with the application, it will deface the font!"));
    }

    setUnsaved();

    ui->ledFontName->setText(tempFont.name);
    ui->ledColorTexture->setText(tempFont.colorTexture);
    ui->ledAlphaTexture->setText(tempFont.alphaTexture);
    ui->sbxMaxHeigthOfChars->setValue(tempFont.maxHeightOfChars);
    ui->sbxDistanceBetweenChars->setValue(tempFont.distanceBetweenChars);

    font = tempFont;

    reloadCharList();
    ui->lvwChars->setCurrentIndex(strListChars->index(0));

    if (font.charList.count() != 0)
    {
        qDebug().noquote() << QString("Font contains %1 chars.").arg(font.charList.count());
        enableFontArea(true);
        reloadCharUI();
    }
    else
    {
        qDebug() << "Font is empty";
        enableFontArea(false);
    }

    font.error = false;
    setUnsaved(false);

    qDebug() << "Font opened.";
}

/// \brief Updates the UI charlist
void wFonts::reloadCharList(bool addChar)
{
    charListUpdate = true;
    qDebug() << "Updating charlist...";
    int currentRow = ui->lvwChars->currentIndex().row();
    qDebug().noquote() << "currentRow:" << currentRow;

    QStringList chars;

    foreach (OTCharacterModel current, font.charList)
        chars << current.character;

    strListChars->setStringList(chars);
    ui->lvwChars->setModel(strListChars);

    if (!addChar)
    {
        if (strListChars->index(currentRow).isValid())
            ui->lvwChars->setCurrentIndex(strListChars->index(currentRow));
        else if (strListChars->index(currentRow - 1).isValid())
            ui->lvwChars->setCurrentIndex(strListChars->index(currentRow - 1));
        else
            ui->lvwChars->setCurrentIndex(strListChars->index(-1));
    }
    else
        ui->lvwChars->setCurrentIndex(strListChars->index(strListChars->rowCount() - 1));


    ui->lblCharCount->setText(QString::number(font.charList.count()));
    charListUpdate = false;
}

/// \brief Saves a font
QString wFonts::save(OTFileMethods::fileMethods method, QString filen)
{
    if (font.charList.count() != 0)
    {
        qDebug() << "Save font...";

        // Get file name via file dialog if no file was open
        if (method != OTFileMethods::backupSave)
        {
            QString dir;
            if (font.path == "")
            {
                if (font.name == "" || font.name == " ")
                    dir = set.read("main", "mainDir").toString() + "/Fonts";
                else
                    dir = set.read("main", "mainDir").toString() + "/Fonts/" + font.name + ".oft";
            }
            else
                dir = font.path;

            if (filen == "" || method == OTFileMethods::saveAs)
            {
                qDebug() << "Save font with file dialog";
                filen = QFileDialog::getSaveFileName(this, tr("Save font"), dir, tr("OMSI font file") +  " (*.oft)");
            }
            if (filen == "")
                return "";

            font.path = filen;
        }

        if ((method != OTFileMethods::backupSave) && (font.path == ""))
            return "";

        if (method != OTFileMethods::backupSave)
        {
            QFile file(font.path);

            // Cut out only file name and put it into the window title
            QFileInfo fileInfo(QFile(font.path).fileName());
            QString filenameWithoutPath(fileInfo.fileName());
            setTitle(filenameWithoutPath);
        }

        OTFontModel tempFont = font;
        if (method == OTFileMethods::backupSave)
            tempFont.path = QDir().absoluteFilePath("backup/font_backup_" + misc.getDate("yyyyMMdd") + "_" + misc.getTime("hhmmss") + " " + font.name + ".oft");

        qDebug() << "Direct path:" << tempFont.path;

        if (!filehandler.saveFont(tempFont, encoding))
        {
            if (method != OTFileMethods::backupSave)
            {
                qWarning() << "Font could not be saved!";
                ui->statusbar->showMessage(tr("Error: The file could not be saved."), 4000);
            }
            return "";
        }


        if (method != OTFileMethods::backupSave)
        {
            saveRecentFiles(QDir().absoluteFilePath(font.path));

            ui->statusbar->showMessage(tr("File saved successfully."), 4000);
            qInfo() << "File successfully saved!";
            qDebug().noquote() << "File: '" + QFileInfo(tempFont.path).absoluteFilePath() + "'";
            setUnsaved(false);
        }
        else
            qDebug().noquote() << "Backup file successfully saved: '" + QFileInfo(tempFont.path).absoluteFilePath() + "'";
    }
    else
        msg.noCharsInFont(this);

    return "";
}

/// \brief Set window title to current font path
void wFonts::setTitle(QString filen)
{
    if (filen == "empty")
        wFonts::setWindowTitle(OTName + " - " + tr("fonts"));
    else
        wFonts::setWindowTitle(OTName + " - " + tr("fonts") + " (" + filen + ")");
}

/// \brief Ceates a new font
void wFonts::on_actionNewFont_triggered()
{
    if (unsaved)
    {
        int msgResult = msg.unsavedContent(this);
        if (msgResult == -1)
            return;
        else if (msgResult == 1)
            save(OTFileMethods::save, font.path);
    }

    qDebug() << "Create new font";

    selectAllAndClear();

    reloadCharList();

    ui->twgFont->setCurrentIndex(0);
    setTitle();
    setUnsaved(false);
}

/// \brief Shows find char section
void wFonts::on_actionFindChar_triggered()
{
    ui->gbxSearchChar->setVisible(true);
    ui->ledSearch->setFocus();
}

/// \brief Jumps to next invalid char
void wFonts::on_actionGoToNextError_triggered()
{
    qDebug() << "Go to next error...";
    int currentPos = ui->lvwChars->currentIndex().row();
    if (currentPos == -1)
        currentPos = 0;

    bool secondRound = false;
    int i = 0;
    if (currentPos == font.charList.count() - 1)
        secondRound = true;
    else if (currentPos == 0)
        i = 1;
    else
        i = currentPos + 1;

    QStringList chars;
    while (true)
    {
        bool rightPixelTooBig = false;
        bool leftPixelTooBig = false;
        bool highestPixelTooBig = false;

        if (QFile(set.read("main", "mainDir").toString() + "/Fonts/" + font.alphaTexture).exists())
        {
            QImage alphaTexture(set.read("main", "mainDir").toString() + "/Fonts/" + font.alphaTexture);

            if (alphaTexture.width() != 0 || alphaTexture.height() != 0)
            {
                if (font.charList.at(i).rightPixel > QString::number(alphaTexture.width()).toInt())
                    rightPixelTooBig = true;

                if (font.charList.at(i).leftPixel > QString::number(alphaTexture.width()).toInt())
                    leftPixelTooBig = true;

                if (font.charList.at(i).highestPixelInFontRow > QString::number(alphaTexture.width()).toInt())
                    highestPixelTooBig = true;
            }
        }

        if (font.charList.at(i).character.isEmpty()               || chars.contains(font.charList.at(i).character) ||
           font.charList.at(i).leftPixel == -1 ||
           font.charList.at(i).rightPixel == -1 ||
           font.charList.at(i).highestPixelInFontRow == -1 ||
           font.charList.at(i).leftPixel >= font.charList.at(i).rightPixel || rightPixelTooBig || leftPixelTooBig || highestPixelTooBig)
        {
            ui->lvwChars->setCurrentIndex(strListChars->index(i));
            reloadCharUI();
            ui->twgFont->setCurrentIndex(1);
            qInfo() << QString("Error in char '%1' at position %2").arg(font.charList.at(i).character).arg(i + 1);
            ui->statusbar->showMessage(tr("Error in character '%1'").arg(font.charList.at(i).character), 5000);
            return;
        }

        chars << font.charList.at(i).character;

        // Wenn es voll ist (1. Runde) ODER wenn in der 2. Runde die vorherige Position erreicht wurde:
        // Überprüfung beenden, da Font keine Fehler enthält.
        if ((secondRound && (i == font.charList.count() - 1)) || (secondRound && (i == currentPos)))
            goto end;

        // Wenn es voll ist / 2. Runde
        if (!secondRound && (i == font.charList.count() - 1))
        {
            i = 0;
            secondRound = true;
        }
        else
            i++;
    }

    end:
    ui->statusbar->showMessage(tr("There aren't any errors. The font is valid."), 4000);
    qInfo() << "Could not found any errors.";
}

/// \brief Deletes selected char
void wFonts::on_actionDeleteSelection_triggered()
{
    if (msg.confirmDeletion(this))
    {
        qDebug().noquote() << QString("Delete char '%1' at position %2...").arg(font.charList.at(ui->lvwChars->currentIndex().row()).character).arg(ui->lvwChars->currentIndex().row());

        font.charList.removeAt(ui->lvwChars->currentIndex().row());
        reloadCharList();

        ui->twgFont->setCurrentIndex(1);

        qDebug() << "Font charlist count:" << font.charList.count();

        if (font.charList.count() == 0)
            selectAllAndClear(true);
        else
            reloadCharUI();

        setUnsaved();
    }
}

/// \brief Shows the settings
void wFonts::on_actionSettings_triggered()
{
    WSETTINGS = new wSettings(this);
    WSETTINGS->setWindowModality(Qt::ApplicationModal);
    WSETTINGS->show();
}

/// \brief Moves char up
void wFonts::on_actionMoveCharUp_triggered()
{
    if (font.charList.count() != 0)
    {
        move(ui->lvwChars->currentIndex().row(), "UP");
        ui->twgFont->setCurrentIndex(1);
    }
}

/// \brief Moves char down
void wFonts::on_actionMoveCharDown_triggered()
{
    if (ui->lvwChars->currentIndex().row() != font.charList.count() - 1)
    {
        move(ui->lvwChars->currentIndex().row(), "DOWN");
        ui->twgFont->setCurrentIndex(1);
    }
}

/// \brief Closes the module
void wFonts::on_actionClose_triggered()
{
    if (unsaved)
    {
        int msgResult = msg.unsavedContent(this);
        if (msgResult == -1)
            return;
        else if (msgResult == 1)
            save(OTFileMethods::save, font.path);
    }

    close();
}

/// \brief Shows fileDialog for color texture
void wFonts::on_btnColorTexture_clicked()
{
     QString filename = QFileDialog::getOpenFileName(this, tr("Select color texture..."), set.read("main", "mainDir").toString() + "/Fonts", tr("Bitmap picture") + " (*.bmp)");
     QFile file(filename);
     if (filename == "")
         return;

     font.colorTexture = filename.remove(0, QString(set.read("main", "mainDir").toString() + "/Fonts").count() + 1);

     if (ui->ledColorTexture->text() != font.colorTexture)
     {
         qDebug() << QString("New color texture: '%1'").arg(filename);
         setUnsaved();
     }

     ui->ledColorTexture->setText(font.colorTexture);
}

/// \brief Shows fileDialog for alpha texture
void wFonts::on_btnAlphaTexture_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Select alpha texture..."), set.read("main", "mainDir").toString() + "/Fonts", tr("Bitmap picture") + " (*.bmp)");
    if (filename == "")
        return;

    font.alphaTexture = filename.remove(0, QString(set.read("main", "mainDir").toString() + "/Fonts").count() + 1);

    if (ui->ledAlphaTexture->text() != font.alphaTexture)
    {
        qDebug() << QString("New alpha texture: '%1'").arg(filename);
        setUnsaved();
    }

    ui->ledAlphaTexture->setText(font.alphaTexture);
}

/// \brief Adds a new char
void wFonts::on_actionNewChar_triggered()
{
    qDebug() << "Add new char...";
    enableFontArea(true);

    font.charList.append(OTCharacterModel());
    reloadCharList(true);
    setUnsaved();

    reloadCharUI();

    ui->twgFont->setCurrentIndex(1);
    ui->ledCharacter->setFocus();
}

/// \brief Loads a template
void wFonts::on_actionLoadTemplate_triggered()
{
    if (unsaved)
    {
        int msgResult = msg.unsavedContent(this);
        if (msgResult == -1)
            return;
        else if (msgResult == 1)
            save(OTFileMethods::save, font.path);
    }

    qDebug() << "Load template...";

    // Fill selection stringlist
    QStringList templates;
    QString pAZ09 = "A-Z, 0-9";
    QString pAZaz09 = "A-Z, a-z, 0-9";
    QString pAZaz09Umlauts = "A-Z, a-z, 0-9 + " + tr("Umlauts");
    QString p09 = "0-9";
    templates << pAZ09 << pAZaz09 << pAZaz09Umlauts << p09;

    bool *selection = new bool;

    QInputDialog selectTemplate;
    QString selectedTemplate = selectTemplate.getItem(this, tr("Select a template"), tr("Template:"), templates, 0, false, selection, Qt::WindowCloseButtonHint);

    // If user pressed OK
    if (*selection == true)
    {
        if (selectedTemplate == pAZ09)
            open(OTFileMethods::silentOpen, ":/rec/data/fontTemplates/A-Z 0-9.oft");

        else if (selectedTemplate == pAZaz09)
            open(OTFileMethods::silentOpen, ":/rec/data/fontTemplates/A-Z a-z 0-9.oft");

        else if (selectedTemplate == pAZaz09Umlauts)
            open(OTFileMethods::silentOpen, ":/rec/data/fontTemplates/A-Z a-z 0-9 Umlauts.oft");

        else if (selectedTemplate == p09)
            open(OTFileMethods::silentOpen, ":/rec/data/fontTemplates/0-9.oft");

        else
        {
            QMessageBox::warning(this, tr("Invalid selection", "Note #1"), tr("The current selection isn't a valid template."));
            qDebug() << "Invalid template!";
        }
        qDebug() << QString("Template: '%1'").arg(selectedTemplate);

        setUnsaved();
    }

    delete selection;
}

/// \brief Switches the encding of the font
void wFonts::on_cobxEncoding_currentTextChanged()
{
    if (ui->cobxEncoding->currentIndex() == 1)
        encoding = "UTF-8";
    else
        encoding = "ANSI";

    setUnsaved();
}

/// \brief Slot for character changes
void wFonts::on_ledCharacter_textChanged(const QString &arg1)
{
    if (font.charList.count() != 0 && ui->lvwChars->currentIndex().row() != -1)
    {
        font.charList[ui->lvwChars->currentIndex().row()].character = arg1;
        if (!charUIUpdate)
            reloadCharList();
    }

    checkCurrentChar();
    setUnsaved();
}

/// \brief Slot for left pixel changes
void wFonts::on_ledLeftPixel_textChanged(const QString &arg1)
{
    if (font.charList.count() != 0 && ui->lvwChars->currentIndex().row() != -1)
    {
        if (arg1 == "")
            font.charList[ui->lvwChars->currentIndex().row()].leftPixel = -1;
        else
            font.charList[ui->lvwChars->currentIndex().row()].leftPixel = arg1.toInt();

        if (!charUIUpdate)
            reloadCharList();
    }

    checkCurrentChar();
    setUnsaved();
}

/// \brief Slot for right pixel changes
void wFonts::on_ledRightPixel_textChanged(const QString &arg1)
{
    if (font.charList.count() != 0 && ui->lvwChars->currentIndex().row() != -1)
    {
        if (arg1 == "")
            font.charList[ui->lvwChars->currentIndex().row()].rightPixel = -1;
        else
            font.charList[ui->lvwChars->currentIndex().row()].rightPixel = arg1.toInt();

        if (!charUIUpdate)
            reloadCharList();
    }

    checkCurrentChar();
    setUnsaved();
}

/// \brief Slot for highestPIFR changes
void wFonts::on_ledHighestPixelInFontRow_textChanged(const QString &arg1)
{
    if (font.charList.count() != 0 && ui->lvwChars->currentIndex().row() != -1)
    {
        if (arg1 == "")
            font.charList[ui->lvwChars->currentIndex().row()].highestPixelInFontRow = -1;
        else
            font.charList[ui->lvwChars->currentIndex().row()].highestPixelInFontRow = arg1.toInt();

        if (!charUIUpdate)
            reloadCharList();
    }

    checkCurrentChar();
    setUnsaved();
}

/// \brief Slot for comment changes
void wFonts::on_ledComment_textChanged(const QString &arg1)
{
    if (font.charList.count() != 0 && ui->lvwChars->currentIndex().row() != -1)
    {
        font.charList[ui->lvwChars->currentIndex().row()].comment = arg1;
        if (!charUIUpdate)
            reloadCharList();
    }

    setUnsaved();
}

/// \brief Sets current font name
void wFonts::on_ledFontName_textChanged(const QString &arg1)
{
    font.name = arg1;
    reloadValidProperty();
    checkCurrentChar();
    setUnsaved();
}

/// \brief Sets current color texture
void wFonts::on_ledColorTexture_textChanged(const QString &arg1)
{
    font.colorTexture = arg1;
    reloadValidProperty();
    checkCurrentChar();
    setUnsaved();
}

/// \brief Sets current alpha texture
void wFonts::on_ledAlphaTexture_textChanged(const QString &arg1)
{
    font.alphaTexture = arg1;
    reloadValidProperty();
    checkCurrentChar();
    setUnsaved();
}

/// \brief Sets current maximum height of characters
void wFonts::on_sbxMaxHeigthOfChars_textChanged(const QString &arg1)
{
    font.maxHeightOfChars = arg1.toInt();
    reloadValidProperty();
    checkCurrentChar();
    setUnsaved();
}

/// \brief Sets current distance between characters
void wFonts::on_sbxDistanceBetweenChars_textChanged(const QString &arg1)
{
    font.distanceBetweenChars = arg1.toInt();
    reloadValidProperty();
    checkCurrentChar();
    setUnsaved();
}

/// \brief Changes UI to current character
void wFonts::on_lvwChars_pressed(const QModelIndex &index)
{
    Q_UNUSED(index);

    reloadCharList();
    reloadCharUI();
}

/// \brief Is connected with a change in the char view
void wFonts::charSelectionChanged(const QModelIndex &newSel, const QModelIndex &oldSel)
{
    Q_UNUSED(newSel);
    Q_UNUSED(oldSel);

    if (!charListUpdate)
        on_lvwChars_pressed(QModelIndex());
}

/// \brief Reloads the char properties
void wFonts::reloadCharUI()
{
    charUIUpdate = true;
    OTCharacterModel character = font.charList.at(ui->lvwChars->currentIndex().row());

    ui->ledCharacter->setText(character.character);

    if (character.leftPixel != -1)
        ui->ledLeftPixel->setText(QString::number(character.leftPixel));
    else
        ui->ledLeftPixel->clear();

    if (character.rightPixel != -1)
        ui->ledRightPixel->setText(QString::number(character.rightPixel));
    else
        ui->ledRightPixel->clear();

    if (character.highestPixelInFontRow != -1)
        ui->ledHighestPixelInFontRow->setText(QString::number(character.highestPixelInFontRow));
    else
        ui->ledHighestPixelInFontRow->clear();

    ui->ledComment->setText(character.comment);

    checkCurrentChar();
    charUIUpdate = false;
}

/// \brief Copy a list of all chars
void wFonts::on_actionCopyChars_triggered()
{
    qInfo() << "Copy chars...";
    QString result;
    foreach (OTCharacterModel current, font.charList)
        result += current.character;

    QClipboard* clipboard = QApplication::clipboard();
    clipboard->setText(result);

    ui->statusbar->showMessage(tr("Characters copied!"), 4000);

}

/// \brief Shows current font file in explorer
void wFonts::on_actionShowInExplorer_triggered()
{
    if (QFile(font.path).exists())
    {
        QStringList args;
        args << "/select," << QDir::toNativeSeparators(font.path);

        QProcess *process = new QProcess(this);
        process->start("explorer.exe", args);
    }
    else
        ui->statusbar->showMessage(tr("The font file (still) doesn't exist."), 4000);
}

/// \brief Finds a char
void wFonts::on_btnFind_clicked()
{
    if (font.charList.count() == 0)
    {
        msg.noCharsInFont(this);
        return;
    }

    QString input = ui->ledSearch->text();
    qDebug().noquote() << "Find char: '" + input + "'";

    currentSearch = input;
    ui->btnNextResult->setEnabled(true);

    // Search for char
    ui->lvwChars->setCurrentIndex(strListChars->index(0));

    for (int i = 0; i < font.charList.count(); i++)
    {
        if (font.charList.at(i).character == input)
        {
            qDebug().noquote() << QString("Char '%1' found at position %2.").arg(input).arg(i + 1);
            reloadCharUI();
            return;
        }

        ui->lvwChars->setCurrentIndex(strListChars->index(i + 1));
    }

    // If char couldn't be found
    QMessageBox::information(this, tr("Character not found", "Note #1"), tr("The entered character could not be found."));
    qDebug() << "Character not found.";
    currentSearch = "";
    ui->btnNextResult->setEnabled(false);
}

/// \brief Finds next search result
void wFonts::on_btnNextResult_clicked()
{
    qDebug() << "Go to next search result";
    if (currentSearch != "")
    {
        if (font.charList.count() == 1)
            return;

        int i = ui->lvwChars->currentIndex().row();

        QList<OTCharacterModel> tempList;
        tempList = font.charList;
        tempList.removeAt(ui->lvwChars->currentIndex().row());

        bool secondRound = false;

        if (i >= tempList.count())
        {
            i = 0;
            qDebug() << "Search: End of font, go to top";
            ui->statusbar->showMessage(tr("The end of the font was reached, search from top"), 4000);
            secondRound = true;
        }

        while (true)
        {
            if (i > tempList.count() - 1)
            {
                qDebug() << "Search: Not other char found";
                ui->statusbar->showMessage(tr("No other character found according to the search criteria."), 4000);
                reloadCharUI();
                return;
            }
            else if (tempList.at(i).character == currentSearch)
            {
                if (i > ui->lvwChars->currentIndex().row() || i == ui->lvwChars->currentIndex().row())
                    i++;
                ui->lvwChars->setCurrentIndex(strListChars->index(i));
                goto end;
            }

            if (!secondRound && (i == tempList.count() - 1))
            {
                i = -1;
                secondRound = true;
            }
            else if (secondRound && (i == ui->lvwChars->currentIndex().row() - 1))
            {
                qDebug() << "Search: Not other char found";
                ui->statusbar->showMessage(tr("No other character found according to the search criteria."), 4000);
                reloadCharUI();
                return;
            }

            i++;
        }
    end:
        if (secondRound)
        {
            qDebug() << "Search: End of font, go to top";
            ui->statusbar->showMessage(tr("The end of the font was reached, search from top"), 4000);
        }

        reloadCharUI();
    }
}

/// \brief Hides the search box
void wFonts::on_btnCloseSearch_clicked()
{
    ui->gbxSearchChar->setVisible(false);
    currentSearch = "";
}

/// \brief Enables or disables the search elements
void wFonts::on_ledSearch_textChanged(const QString &arg1)
{
    ui->btnFind->setEnabled(arg1.count() == 1);
    ui->btnNextResult->setEnabled(arg1.count() == 1);
    currentSearch = arg1;
}

/// \brief Searchs a char
void wFonts::on_ledSearch_returnPressed()
{
    on_btnFind_clicked();
}

/// \brief Opens bug report module
void wFonts::on_actionSendFeedback_triggered()
{
    OTMiscellaneous::sendFeedback();
}
