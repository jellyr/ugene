/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2012 UniPro <ugene@unipro.ru>
 * http://ugene.unipro.ru
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#include "GTTestsToggleView.h"
#include "api/GTGlobals.h"
#include "api/GTKeyboardDriver.h"
#include "api/GTMouseDriver.h"
#include "api/GTMenu.h"
#include "api/GTFileDialog.h"
#include "api/GTTreeWidget.h"
#include "GTUtilsProject.h"
#include "GTUtilsDocument.h"
#include "GTUtilsLog.h"
#include "GTUtilsApp.h"
#include "GTUtilsToolTip.h"
#include "GTUtilsDialogRunnables.h"
#include "GTUtilsMdi.h"
#include "GTUtilsAnnotationsTreeView.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsTaskTreeView.h"
#include "GTUtilsSequenceView.h"
#include "GTUtilsMdi.h"

#include <U2Core/DocumentModel.h>
#include <U2View/AnnotatedDNAViewFactory.h>
#include <U2View/MSAEditorFactory.h>
#include <U2View/ADVConstants.h>
#include <QClipboard>

namespace U2 {

namespace GUITest_common_scenarios_toggle_view {

GUI_TEST_CLASS_DEFINITION(test_0005) {

// 1. Use menu {File->Open}. Open file _common_data/scenarios/project/multiple.fa, open file as separate sequences
    Runnable *dialog = new GTUtilsDialogRunnables::SequenceReadingModeSelectorDialogFiller(os);
    GTUtilsDialog::waitForDialog(os, dialog, GUIDialogWaiter::Modal);
    GTUtilsProject::openFiles(os, testDir + "_common_data/scenarios/project/multiple.fa");
    GTGlobals::sleep();

    GTGlobals::sleep();
    QWidget *overViewSe1 = GTWidget::findWidget(os, "overview_se1");
    QWidget *overViewSe2 = GTWidget::findWidget(os, "overview_se2");
    QWidget *toggleViewButton = GTWidget::findWidget(os, "toggleViewButton");
    QWidget *toggleViewButtonSe1 = GTWidget::findWidget(os, "toggle_view_button_se1");
    QWidget *toggleViewButtonSe2 = GTWidget::findWidget(os, "toggle_view_button_se2");

// 2. Click on toolbar button Toggle views. Click menu item Hide all overviews.
    Runnable *chooser0 = new GTUtilsDialogRunnables::PopupChooser(os, QStringList() << "toggleOverview");
    GTUtilsDialog::waitForDialog(os, chooser0, GUIDialogWaiter::Popup);
    GTWidget::click(os, toggleViewButton);
    GTGlobals::sleep();

// Expected state: panoramical views for both sequences has been closed, all zoom buttons at sequence views becomes disabled
    GTGlobals::sleep();
    CHECK_SET_ERR(overViewSe1->isHidden() && overViewSe2->isHidden(),
        "panoramical views for both sequences has not been closed");

// 3. Click on toolbar button Toggle views. Click menu item Show all overviews.
    Runnable *chooser1 = new GTUtilsDialogRunnables::PopupChooser(os, QStringList() << "toggleOverview");
    GTUtilsDialog::waitForDialog(os, chooser1, GUIDialogWaiter::Popup);
    GTWidget::click(os, toggleViewButton);
    GTGlobals::sleep();

    QWidget *_overViewSe1 = GTWidget::findWidget(os, "overview_se1");
    QWidget *_overViewSe2 = GTWidget::findWidget(os, "overview_se2");

// Expected state: panoramical view for both sequences has been appeared
    GTGlobals::sleep();
    CHECK_SET_ERR(!_overViewSe1->isHidden() && !_overViewSe2->isHidden(), 
        "panoramical view for both sequences has not been shown");
}

GUI_TEST_CLASS_DEFINITION(test_0006) {
// 1. Use menu {File->Open}. Open file _common_data/scenarios/project/multiple.fa
    Runnable *dialog = new GTUtilsDialogRunnables::SequenceReadingModeSelectorDialogFiller(os);
    GTUtilsDialog::waitForDialog(os, dialog, GUIDialogWaiter::Modal);
    GTUtilsProject::openFiles(os, testDir + "_common_data/scenarios/project/multiple.fa");
    GTGlobals::sleep();

    GTGlobals::sleep();
    QWidget *detailsViewSe1 = GTWidget::findWidget(os, "det_view_se1");
    QWidget *detailsViewSe2 = GTWidget::findWidget(os, "det_view_se2");
    QWidget *toolBarSe1 = GTWidget::findWidget(os, "tool_bar_se1");
    QWidget *toolBarSe2 = GTWidget::findWidget(os, "tool_bar_se2");
    QWidget *toggleViewButton = GTWidget::findWidget(os, "toggleViewButton");
    QWidget *toggleViewButtonSe2 = GTWidget::findWidget(os, "toggle_view_button_se2");
    QWidget *toggleViewButtonSe1 = GTWidget::findWidget(os, "toggle_view_button_se1");

// 2. Click on toolbar button Toggle views. Click menu item Hide all details.
    Runnable *chooser0 = new GTUtilsDialogRunnables::PopupChooser(os, QStringList() << "toggleDetailsView");
    GTUtilsDialog::waitForDialog(os, chooser0, GUIDialogWaiter::Popup);
    GTWidget::click(os, toggleViewButton);
    GTGlobals::sleep();

// Expected state: views for both sequences has been closed, but toolbars still present.
    GTGlobals::sleep();
    CHECK_SET_ERR(detailsViewSe1->isHidden() && detailsViewSe2->isHidden(),
        "details views for both sequences has not been closed");
    CHECK_SET_ERR(!toolBarSe1->isHidden() && !toolBarSe2->isHidden(), 
        "toolbars for both sequences has not been shown");

// 3. Click on toolbar button Toggle views. Click menu item Show all details.
    Runnable *chooser1 = new GTUtilsDialogRunnables::PopupChooser(os, QStringList() << "toggleDetailsView");
    GTUtilsDialog::waitForDialog(os, chooser1, GUIDialogWaiter::Popup);
    GTWidget::click(os, toggleViewButton);
    GTGlobals::sleep();

// Expected state: view for both sequences has been appeared
    GTGlobals::sleep();
    CHECK_SET_ERR(!detailsViewSe1->isHidden() && !detailsViewSe2->isHidden(), 
        "details view for both sequences has not been shown");
}

GUI_TEST_CLASS_DEFINITION(test_0007) {

// 1. Use menu {File->Open}. Open file _common_data/scenarios/project/multiple.fa
    Runnable *dialog = new GTUtilsDialogRunnables::SequenceReadingModeSelectorDialogFiller(os);
    GTUtilsDialog::waitForDialog(os, dialog, GUIDialogWaiter::Modal);
    GTUtilsProject::openFiles(os, testDir + "_common_data/scenarios/project/multiple.fa");
    GTGlobals::sleep();

    GTGlobals::sleep();
    QWidget *overViewSe1 = GTWidget::findWidget(os, "overview_se1");
    QWidget *overViewSe2 = GTWidget::findWidget(os, "overview_se2");
    QWidget *DetailsViewSe1 = GTWidget::findWidget(os, "det_view_se1");
    QWidget *DetailsViewSe2 = GTWidget::findWidget(os, "det_view_se2");
    QWidget *zoomViewSe1 = GTWidget::findWidget(os, "pan_view_se1");
    QWidget *zoomViewSe2 = GTWidget::findWidget(os, "pan_view_se2");
    QWidget *toolBarSe1 = GTWidget::findWidget(os, "tool_bar_se1");
    QWidget *toolBarSe2 = GTWidget::findWidget(os, "tool_bar_se2");
    QWidget *toggleViewButton = GTWidget::findWidget(os, "toggleViewButton");
    QWidget *toggleViewButtonSe1 = GTWidget::findWidget(os, "toggle_view_button_se1");
    QWidget *toggleViewButtonSe2 = GTWidget::findWidget(os, "toggle_view_button_se2");

// 2. Click on toolbar button Toggle views. Click menu item Hide all sequences.
    Runnable *chooser0 = new GTUtilsDialogRunnables::PopupChooser(os, QStringList() << "toggleAllSequenceViews");
    GTUtilsDialog::waitForDialog(os, chooser0, GUIDialogWaiter::Popup);
    GTWidget::click(os, toggleViewButton);
    GTGlobals::sleep();

// Expected state: all views for both sequences has been closed, but toolbars still present.
    GTGlobals::sleep();
    CHECK_SET_ERR(overViewSe1->isHidden() && overViewSe2->isHidden(),
        "panoramical views for both sequences has not been closed");
    CHECK_SET_ERR(DetailsViewSe1->isHidden() && DetailsViewSe2->isHidden(),
        "details views for both sequences has not been closed");
    CHECK_SET_ERR(zoomViewSe1->isHidden() && zoomViewSe2->isHidden(),
        "zoom views for both sequences has not been closed");
    CHECK_SET_ERR(!toolBarSe1->isHidden() && !toolBarSe2->isHidden(), 
        "toolbars view for both sequences has not been shown");

// 3. Click on toolbar button Toggle views. Click menu item Show all sequences.
    Runnable *chooser1 = new GTUtilsDialogRunnables::PopupChooser(os, QStringList() << "toggleAllSequenceViews");
    GTUtilsDialog::waitForDialog(os, chooser1, GUIDialogWaiter::Popup);
    GTWidget::click(os, toggleViewButton);
    GTGlobals::sleep();

// Expected state: all views for both sequences has been appeared
    GTGlobals::sleep();
    CHECK_SET_ERR(!overViewSe1->isHidden() && !overViewSe2->isHidden(), 
        "panoramical view for both sequences has not been shown");
    CHECK_SET_ERR(!DetailsViewSe1->isHidden() && !DetailsViewSe2->isHidden(), 
        "details view for both sequences has not been shown");
    CHECK_SET_ERR(!zoomViewSe1->isHidden() && !zoomViewSe2->isHidden(), 
        "zoom view for both sequences has not been shown");
}

GUI_TEST_CLASS_DEFINITION(test_0008) {
// 1. Use menu {File->Open}. Open file _common_data/scenarios/project/multiple.fa
    Runnable *dialog = new GTUtilsDialogRunnables::SequenceReadingModeSelectorDialogFiller(os);
    GTUtilsDialog::waitForDialog(os, dialog, GUIDialogWaiter::Modal);
    GTUtilsProject::openFiles(os, testDir + "_common_data/scenarios/project/multiple.fa");
    GTGlobals::sleep();

    GTGlobals::sleep();
    QWidget *mainWindow = AppContext::getMainWindow()->getQMainWindow();
    QToolBar *toolBarSe2 = mainWindow->findChild<QToolBar*>("tool_bar_se2");
    CHECK_SET_ERR(mainWindow != NULL, "Main Window not found");
    CHECK_SET_ERR(toolBarSe2 != NULL, "Tool bar not found");

    QAction *zoomAction = mainWindow->findChild<QAction*>("zoom_to_range_se2");
    CHECK_SET_ERR(zoomAction != NULL, "Zoom to range action not found");
    QWidget *zoomButton = toolBarSe2->widgetForAction(zoomAction);
    CHECK_SET_ERR(zoomButton != NULL, "Zoom button not found");

    QAction *zoomInSe2 = mainWindow->findChild<QAction*>("action_zoom_in_se2");
    QAction *zoomOutSe2 = mainWindow->findChild<QAction*>("action_zoom_out_se2");
    CHECK_SET_ERR(zoomInSe2 != NULL, "Zoom In action not found");
    CHECK_SET_ERR(zoomOutSe2 != NULL, "Zoom Out action not found");

    QWidget *zoomInButton = toolBarSe2->widgetForAction(zoomInSe2);
    QWidget *zoomOutButton = toolBarSe2->widgetForAction(zoomInSe2);

    CHECK_SET_ERR(zoomInButton != NULL, "Zoom In button not found");
    CHECK_SET_ERR(zoomOutButton != NULL, "Zoom Out button not found");

// 2. Click on toolbar button Zoom to range for sequence se2. Select region 20..50, then click OK.
    Runnable *filler = new GTUtilsDialogRunnables::ZoomToRangeDialogFiller(os, 20, 50);
    GTUtilsDialog::waitForDialog(os, filler);
    GTWidget::click(os, zoomButton);
    GTGlobals::sleep();

// Expected state: sequence overview show 20..50 sequence region, all zoom buttons at this toolbar becomes enabled
    GTGlobals::sleep();
    //  TODO: Expected state: sequence overview showh 20..50 sequence region
    CHECK_SET_ERR(zoomInButton->isEnabled(), "Zoom In button is not enabled");
    CHECK_SET_ERR(zoomOutButton->isEnabled(), "Zoom Out button is not enabled");

// 3. Click on toolbar button Toggle view for sequence se2. Click menu item Hide zoom view.
    Runnable *chooser0 = new GTUtilsDialogRunnables::PopupChooser(os, QStringList() << "show_hide_zoom_view");
    GTUtilsDialog::waitForDialog(os, chooser0, GUIDialogWaiter::Popup);
    GTWidget::click(os, GTWidget::findWidget(os, "toggle_view_button_se2"));
    GTGlobals::sleep();

// Expected state: panoramical view for se2 sequence has been closed, all zoom buttons at this toolbar becomes disabled
    GTGlobals::sleep();
    QWidget *zoomViewSe2 = GTWidget::findWidget(os, "pan_view_se2");
    CHECK_SET_ERR(zoomViewSe2 != NULL, "Zoom View widget not found");
    CHECK_SET_ERR(zoomViewSe2->isHidden(), "panoramical view for se2 sequence has been not closed");
    CHECK_SET_ERR(!zoomInButton->isEnabled(), "Zoom In button is enabled");
    CHECK_SET_ERR(!zoomOutButton->isEnabled(), "Zoom Out button is enabled");

// 4. Click on toolbar button Toggle view for sequence se2. Click menu item Show zoom view.
    Runnable *chooser1 = new GTUtilsDialogRunnables::PopupChooser(os, QStringList() << "show_hide_zoom_view");
    GTUtilsDialog::waitForDialog(os, chooser1, GUIDialogWaiter::Popup);
    GTWidget::click(os, GTWidget::findWidget(os, "toggle_view_button_se2"));
    GTGlobals::sleep();

// Expected state: panoramical view for se2 sequence has been appeared, all zoom buttons at this toolbar becomes enabled
    GTGlobals::sleep();
    CHECK_SET_ERR(!zoomViewSe2->isHidden(), "panoramical view for se2 sequence has been not appeared");
    CHECK_SET_ERR(zoomInButton->isEnabled(), "Zoom In button is not enabled");
    CHECK_SET_ERR(zoomOutButton->isEnabled(), "Zoom Out button is not enabled");
}

GUI_TEST_CLASS_DEFINITION(test_0009)
{
    Runnable *dialog = new GTUtilsDialogRunnables::SequenceReadingModeSelectorDialogFiller(os);
    GTUtilsDialog::waitForDialog(os, dialog, GUIDialogWaiter::Modal);
    GTUtilsProject::openFiles(os, testDir + "_common_data/scenarios/project/multiple.fa");
    GTGlobals::sleep(1000);


    QWidget *overViewSe1 = GTWidget::findWidget(os, "overview_se1");
    QWidget *overViewSe2 = GTWidget::findWidget(os, "overview_se2");

    QWidget *toggleViewButton = GTWidget::findWidget(os, "toggleViewButton");
    QWidget *toggleViewButtonSe2 = GTWidget::findWidget(os, "toggle_view_button_se2");

    Runnable *chooser0 = new GTUtilsDialogRunnables::PopupChooser(os, QStringList() << "show_hide_overview");
    GTUtilsDialog::waitForDialog(os, chooser0, GUIDialogWaiter::Popup);
    GTWidget::click(os, toggleViewButtonSe2);
    GTGlobals::sleep(1000);
    GTGlobals::sleep(1000);
    CHECK_SET_ERR(overViewSe2->isHidden(), "panoramical view for se2 sequence has been not closed");

    Runnable *chooser1 = new GTUtilsDialogRunnables::PopupChooser(os, QStringList() << "toggleOverview");
    GTUtilsDialog::waitForDialog(os, chooser1, GUIDialogWaiter::Popup);
    GTWidget::click(os, toggleViewButton);
    GTGlobals::sleep(1000);
    GTGlobals::sleep(1000);
    CHECK_SET_ERR(overViewSe1->isHidden() && overViewSe2->isHidden(),
                  "panoramical views for both sequences has been not closed");

    Runnable *chooser2 = new GTUtilsDialogRunnables::PopupChooser(os, QStringList() << "show_hide_overview");
    GTUtilsDialog::waitForDialog(os, chooser2, GUIDialogWaiter::Popup);
    GTWidget::click(os, toggleViewButtonSe2);
    GTGlobals::sleep(1000);
    GTGlobals::sleep(1000);
    CHECK_SET_ERR(!overViewSe2->isHidden(), "panoramical view for se2 sequence has been not shown");

    Runnable *chooser3 = new GTUtilsDialogRunnables::PopupChooser(os, QStringList() << "toggleOverview");
    GTUtilsDialog::waitForDialog(os, chooser3, GUIDialogWaiter::Popup);
    GTWidget::click(os, toggleViewButton);
    GTGlobals::sleep(1000);
    GTGlobals::sleep(10000);
    CHECK_SET_ERR(overViewSe1->isHidden() && overViewSe2->isHidden(),
                  "panoramical view for both sequences has been not closed");
}


GUI_TEST_CLASS_DEFINITION(test_0010)
{
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTGlobals::sleep(1000);

    GTUtilsSequenceView::selectSequenceRegion(os, 60000, 70000);
    GTGlobals::sleep(1000);

}

} // namespace
} // namespace U2
