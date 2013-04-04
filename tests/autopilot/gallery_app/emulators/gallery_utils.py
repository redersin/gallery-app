# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2013 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

from time import sleep

class GalleryUtils(object):
    """An emulator class that makes it easy to interact with
    general components of the gallery app.
    """

    def __init__(self, app):
        self.app = app

    def select_single_retry(self, object_type, **kwargs):
        """Returns the item that is searched for with app.select_single"""
        """In case of the item was not found (not created yet) a second attempt is taken 1 second later"""
        """Makes searching for dialogs more robust (which are created on clicks for example)"""
        item = self.app.select_single(object_type, **kwargs)
        if item == None:
            sleep(1)
            item = self.app.select_single(object_type, **kwargs)
        return item


    def get_qml_view(self):
        """Get the main QML view"""
        return self.app.select_single("QQuickView")

    def get_main_photo_viewer_loader(self):
        """Returns the loder item for the PhotoViewer."""
        return self.app.select_single("QQuickLoader", objectName="photoViewerLoader")

    def get_main_photo_viewer(self):
        """Returns the PhotoViewer."""
        return self.select_single_retry("PhotoViewer", objectName="photoViewer")


    def get_tabs_bar(self):
        """Returns the top tabs bar."""
        return self.app.select_single("NewTabBar")

    def get_albums_tab_button(self):
        """Returns the photos tab."""
        return self.app.select_single("AbstractButton", buttonIndex=0)


    def get_toolbar(self):
        """Returns the toolbar in the main events view."""
        main_view = self.app.select_single("MainScreen", objectName="overview")
        return main_view.get_children_by_type("Toolbar")[0]

    def get_toolbar_button(self, button_idx):
        """Returns the button with index idx from the toolbar"""
        toolbar = self.get_toolbar()
        item = toolbar.get_children_by_type("QQuickItem")[0]
        row = item.get_children_by_type("QQuickRow")[0]
        button_loaders = row.get_children_by_type("QQuickLoader")
        if len(button_loaders) > 0:
            # new toolbar
            button_loader = button_loaders[button_idx]
            return button_loader.get_children_by_type("Button")[0]
        else:
            # old toolbar
            return row.get_children_by_type("Button")[button_idx]

    def get_toolbar_cancel_icon(self):
        """Returns the cancel icon of the events view."""
        toolbar = self.get_toolbar()
        item = toolbar.get_children_by_type("QQuickItem")[0]
        back_loaders = item.get_children_by_type("QQuickLoader")
        if len(back_loaders) > 0:
            # new toolbar
            back_loader = back_loaders[0]
            return back_loader.get_children_by_type("Button")[0]
        else:
            # old toolbar
            return item.get_children_by_type("Button")[0]


    def get_delete_dialog(self):
        """Returns the delete dialog in the events view."""
        return self.select_single_retry("DeleteDialog", objectName="deleteDialog")

    def get_delete_dialog_delete_button(self):
        """Returns the delete button of the delete popover."""
        return self.app.select_single("Button", objectName="deleteDialogYes", visible=True)

    def get_delete_dialog_cancel_button(self):
        """Returns the cancel button of the delete popover."""
        return self.app.select_single("Button", objectName="deleteDialogNo", visible=True)


    def get_first_event(self):
        """Returns the first event in the event view"""
        return self.app.select_single("OrganicMediaList", objectName="organicEventItem0")

    def get_first_image_in_event_view(self):
        """Returns the first photo of the gallery."""
        event = self.get_first_event()
        list_view = event.get_children_by_type("QQuickListView")[0]
        item = list_view.get_children_by_type("QQuickItem")[0]
        first_photo_delegate = item.get_children_by_type("QQuickItem", objectName="eventPhoto")[0]
        first_photo = first_photo_delegate.get_children_by_type("UbuntuShape")[0]
        return first_photo


    def get_first_album(self):
        """Returns the first album in the albums view"""
        return self.app.select_many("CheckerboardDelegate", objectName="checkerboardDelegate")[0]
