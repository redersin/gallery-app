# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2013 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

from ubuntuuitoolkit import emulators as toolkit_emulators


class AlbumEditorAnimated(toolkit_emulators.UbuntuUIToolkitEmulatorBase):
    """An emulator class that makes it easy to interact with the album editor"""

    def close(self):
        cover_image = self.album_cover_image()
        # click left of the cover
        x, y, w, h = cover_image.globalRect
        self.pointing_device.move(x + w/2, y + h + 1)
        self.pointing_device.click()
        self.ensure_fully_closed

    def ensure_fully_open(self):
        self.animationRunning.wait_for(False)
        self.isOpen.wait_for(True)

    def ensure_fully_closed(self):
        self.isOpen.wait_for(False)
        self.animationRunning.wait_for(False)

    def album_editor(self):
        """Returns the album editor."""
        return self.select_single("AlbumEditor", objectName="mainAlbumEditor")

    def album_title_entry_field(self):
        """Returns the album title input box."""
        return self.select_many("TextEditOnClick",
                                objectName="albumTitleField")[0]

    def click_title_field(self):
        self.pointing_device.click_object(self.album_title_entry_field())

    def album_subtitle_entry_field(self):
        """Returns the album subtitle input box."""
        return self.select_many("TextEditOnClick",
                                objectName="albumSubtitleField")[0]

    def click_subtitle_field(self):
        self.pointing_device.click_object(self.album_subtitle_entry_field())

    def album_cover_image(self):
        return self.select_many("QQuickImage", objectName="albumCoverImage")[0]

    def add_photos(self):
        self.pointing_device.click_object(self._plus_icon())

    def _plus_icon(self):
        """Returns the plus icon to add photos"""
        return self.select_single("QQuickImage",
                                  objectName="albumCoverAddPhotosImage",
                                  visible=True)
