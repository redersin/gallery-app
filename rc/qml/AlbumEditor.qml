/*
 * Copyright (C) 2011 Canonical Ltd
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
 * Charles Lindsay <chaz@yorba.org>
 */

import QtQuick 1.1
import Gallery 1.0
import "../Capetown"
import "GalleryUtility.js" as GalleryUtility

Item {
  id: albumEditor

  signal closeRequested(variant album, bool enterViewer)

  property Album album

  // readonly
  property variant editorRect

  // internal
  property real canonicalWidth: gu(66)
  property real canonicalHeight: gu(80)

  function editNewAlbum() {
    albumEditor.album = albumModel.createOrphan();
    coverMenu.state = "hidden"
  }

  function editAlbum(album) {
    albumEditor.album = album;
    coverMenu.state = "hidden"
  }

  // internal
  function closeAlbum() {
    if (album.contentPageCount > 0) {
      albumModel.addOrphan(album);

      // Don't want to stay on the cover.
      if (album.currentPage == album.firstValidCurrentPage)
        album.currentPage = album.firstContentPage;
    } else {
      albumModel.destroyOrphan(album);
    }
  }

  // internal
  function resetEditorRect() {
    editorRect = GalleryUtility.getRectRelativeTo(cover, albumEditor);
  }

  onAlbumChanged: resetEditorRect() // HACK: works, but not conceptually correct.
  onWidthChanged: resetEditorRect()
  onHeightChanged: resetEditorRect()

  AlbumCollectionModel {
    id: albumModel
  }

  MouseArea {
    id: blocker

    anchors.fill: parent
  }

  AspectArea {
    id: coverArea

    anchors.centerIn: parent
    width: Math.min(parent.width - gu(8), canonicalWidth)
    height: Math.min(parent.height - gu(8), canonicalHeight)

    aspectWidth: canonicalWidth
    aspectHeight: canonicalHeight

    content: AlbumCover {
      id: cover

      anchors.fill: parent

      album: albumEditor.album
      isPreview: false

      MouseArea {
        anchors.fill: parent

        onPressed: coverMenu.flipVisibility()
      }

      MouseArea {
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        width: gu(14)
        height: gu(14)
        onClicked: mediaSelector.show()
      }

      Image {
        anchors.verticalCenter: parent.top
        anchors.horizontalCenter: parent.right

        source: "../img/album-edit-close.png"

        MouseArea {
          anchors.fill: parent
          onClicked: {
            closeAlbum();

            albumEditor.closeRequested(albumEditor.album, false);
          }
        }
      }
    }
  }
  
  // Cover picker
  AlbumCoverMenu {
    id: coverMenu
    
    visible: false
    state: "hidden"
    popupOriginX: -gu(3)
    popupOriginY: -gu(15)
    
    onActionInvoked: {
      albumEditor.album.coverNickname = name
      state = "hidden"
    }
  }

  MouseArea {
    id: menuCancelArea

    anchors.fill: parent
    visible: coverMenu.state != "hidden"
    onPressed: coverMenu.state = "hidden"
  }

  MediaSelector {
    id: mediaSelector

    anchors.fill: parent

    album: albumEditor.album

    onCancelRequested: hide()

    onDoneRequested: {
      album.addSelectedMediaSources(model);
      closeAlbum();

      albumEditor.closeRequested(albumEditor.album, true);
      hide();
    }
  }
}
