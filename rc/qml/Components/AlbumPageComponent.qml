/*
 * Copyright (C) 2012 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 * Jim Nelson <jim@yorba.org>
 * Charles Lindsay <chaz@yorba.org>
 */

import QtQuick 1.1
import Gallery 1.0
import "AlbumInternals"

// One "piece of paper" in an album.  Contains album content i.e. photos (or
// the cover) from the album and handles rotating, displaying an arbitrary page
// on the front (right) or back (left) sides.
Flipable {
  id: albumPageComponent

  property Album album // The album this page is in.

  property alias frontPage: frontContents.page // On the right when viewed like in a book.
  property alias backPage: backContents.page // On the left when viewed like in a book.

  // [0,2]: 0 = flat right page, 1 = flat left page, 2 = back to right page.
  // The page turns 360 degrees from 0-2 in a normal "book" fashion.
  property real flipFraction: 0

  property bool isPreview: false // Whether to load preview or normal images.
  // These override the isPreview option if specified.
  property alias frontIsPreview: frontContents.isPreview
  property alias backIsPreview: backContents.isPreview

  // Whether to draw a frame around the page, if it's a content page.
  property bool contentHasPreviewFrame: false

  property real topMargin: pageTopMargin
  property real bottomMargin: pageBottomMargin
  property real gutterMargin: pageGutterMargin
  property real outerMargin: pageOuterMargin
  property real insideMargin: pageInsideMargin

  // readonly
  property alias frontIsCover: frontContents.isCover
  property alias backIsCover: backContents.isCover
  property alias frameToContentWidth: frontContents.frameToContentWidth
  property alias frameToContentHeight: frontContents.frameToContentHeight

  property real pageTopMargin: gu(6)
  property real pageBottomMargin: gu(6)
  property real pageGutterMargin: gu(2) // Between spine and photo.
  property real pageOuterMargin: gu(3) // Between opposite edge and photo.
  property real pageInsideMargin: gu(4) // Between photos on one page.

  property real previewTopMargin: gu(2)
  property real previewBottomMargin: gu(2)
  property real previewGutterMargin: gu(1) // Between spine and photo.
  property real previewOuterMargin: gu(1) // Between opposite edge and photo.
  property real previewInsideMargin: gu(2) // Between photos on one page.

  // Returns the page number of the left page of the current "spread", denoted
  // by a currentPage number from an album.  This will go on the backPage.
  function leftPageForCurrent(currentPage) {
    return currentPage;
  }

  // Returns the page number of the right page of the current "spread", denoted
  // by a currentPage number from an album.  This will go on the frontPage.
  function rightPageForCurrent(currentPage) {
    return currentPage + 1;
  }

  front: AlbumPageContents {
    id: frontContents

    width: albumPageComponent.width
    height: albumPageComponent.height

    album: albumPageComponent.album

    isPreview: albumPageComponent.isPreview
    contentHasPreviewFrame: albumPageComponent.contentHasPreviewFrame

    topMargin: albumPageComponent.topMargin
    bottomMargin: albumPageComponent.bottomMargin
    gutterMargin: albumPageComponent.gutterMargin
    outerMargin: albumPageComponent.outerMargin
    insideMargin: albumPageComponent.insideMargin
  }

  back: AlbumPageContents {
    id: backContents

    width: albumPageComponent.width
    height: albumPageComponent.height

    album: albumPageComponent.album

    isPreview: albumPageComponent.isPreview
    contentHasPreviewFrame: albumPageComponent.contentHasPreviewFrame

    topMargin: albumPageComponent.topMargin
    bottomMargin: albumPageComponent.bottomMargin
    gutterMargin: albumPageComponent.gutterMargin
    outerMargin: albumPageComponent.outerMargin
    insideMargin: albumPageComponent.insideMargin
  }

  transform: Rotation {
    id: rotation

    origin.x: 0
    origin.y: albumPageComponent.height / 2

    axis.x: 0
    axis.y: 1
    axis.z: 0

    angle: (flipFraction * -180)
  }
}