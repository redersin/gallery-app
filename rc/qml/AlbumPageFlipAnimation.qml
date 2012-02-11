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
 *
 * Inspired by code written by Tommi Laukkanen, available at
 * http://www.substanceofcode.com/downloads/flipboard_page_turn_with_qml.zip
 */

import QtQuick 1.1
import Gallery 1.0

Rectangle {
  id: pageFlipAnimation
  
  property Album album
  property int durationMsec: 1000
  
  // readonly
  property bool pageFlipped: false
  property bool isRunning: (flippable.state != "" || clipper.trigger)
  
  // internal
  property AlbumPage leftPage
  property AlbumPage rightPage
  property bool leftToRight: true
  property bool leftIsCover: false
  property bool rightIsCover: false
  property int finalPageNumber
  
  // Moves to selected page without any animation (good for initializing view)
  function setTo(index) {
    if (isRunning) {
      console.log("Blocking set of album page", index);
      
      return;
    }
    
    if (index < 0) {
      leftIsCover = true;
      rightIsCover = true;
    } else {
      leftIsCover = false;
      rightIsCover = false;
      leftPage = album.getPage(index);
      rightPage = leftPage;
    }
    
    clipper.visible = false;
  }
  
  // Animates page flip and saves new page number to album.
  function turnTo(index) {
    if (isRunning) {
      console.log("Blocking turn to album page", index);
      
      return;
    }
    
    if (index < 0) {
      close();
      
      return;
    }
    
    if (index == album.currentPageNumber || index >= album.pageCount)
      return;
    
    leftIsCover = false;
    rightIsCover = false;
    
    if (album.isClosed) {
      leftIsCover = true;
      rightPage = album.getPage(index);
      leftToRight = true;
    } else if (album.currentPageNumber > index) {
      leftPage = album.getPage(index);
      rightPage = album.getPage(album.currentPageNumber);
      leftToRight = false;
    } else {
      leftPage = album.getPage(album.currentPageNumber);
      rightPage = album.getPage(index);
      leftToRight = true;
    }
    
    finalPageNumber = index;
    
    start();
  }
  
  function close() {
    if (isRunning) {
      console.log("Blocking close of album");
      
      return;
    }
    
    if (album.isClosed)
      return;
    
    leftIsCover = true;
    rightPage = album.getPage(album.currentPageNumber);
    leftToRight = false;
    finalPageNumber = -1;
    
    start();
  }
  
  function start() {
    if (isRunning) {
      console.log("Blocking start of page flip animation");
      
      return;
    }
    
    // reset
    pageFlipped = false;
    clipper.trigger = false;
    
    clipper.visible = true;
    
    clipper.x = leftToRight ? pageFlipAnimation.width / 2 : 0
    flippable.x = leftToRight ? -(pageFlipAnimation.width / 2) : 0
    flippable.rotation.angle = leftToRight ? 0.0 : -180.0;
    
    // start animation
    clipper.trigger = true;
  }
  
  onAlbumChanged: {
    if (album)
      setTo(album.currentPageNumber);
  }
  
  Item {
    width: parent.width / 2
    height: parent.height
    
    clip: true
    
    Rectangle {
      x: 0
      y: 0
      width: parent.width
      height: parent.height
      
      AlbumCover {
        id: coverBackground
        
        album: pageFlipAnimation.album
        
        width: pageFlipAnimation.width
        height: pageFlipAnimation.height
        
        visible: leftIsCover
      }
      
      AlbumPageComponent {
        id: leftPageBackground
        
        albumPage: leftPage
        
        width: pageFlipAnimation.width
        height: pageFlipAnimation.height
        
        visible: !leftIsCover
      }
    }
  }
  
  Item {
    x: parent.width / 2
    width: parent.width / 2
    height: parent.height
    
    clip: true
    
    Rectangle {
      x: -(parent.width / 2)
      y: 0
      width: parent.width
      height: parent.height
      
      AlbumCover {
        id: rightCoverBackground
        
        album: pageFlipAnimation.album
        
        x: parent.x
        y: parent.y
        width: pageFlipAnimation.width
        height: pageFlipAnimation.height
        
        visible: rightIsCover
      }
      
      AlbumPageComponent {
        id: rightPageBackground
        
        albumPage: rightPage
        
        x: parent.x
        y: parent.y
        width: pageFlipAnimation.width
        height: pageFlipAnimation.height
        
        visible: !rightIsCover
      }
    }
  }
  
  Item {
    id: clipper
    
    property bool trigger: false
    
    x: leftToRight ? parent.width / 2 : 0
    width: parent.width / 2
    height: parent.height
    
    clip: true
    
    Flipable {
      id: flippable
      
      x: leftToRight ? -(pageFlipAnimation.width / 2) : 0
      width: pageFlipAnimation.width
      height: pageFlipAnimation.height
      
      front: Rectangle {
        x: 0
        y: 0
        width: parent.width
        height: parent.height
        
        AlbumCover {
          id: coverAnimated
          
          album: pageFlipAnimation.album
          
          width: pageFlipAnimation.width
          height: pageFlipAnimation.height
          
          visible: leftIsCover
        }
        
        AlbumPageComponent {
          id: leftPageAnimated
          
          albumPage: leftPage
          
          width: pageFlipAnimation.width
          height: pageFlipAnimation.height
          
          visible: !leftIsCover
        }
      }
      
      back: Rectangle {
        x: 0
        y: 0
        width: parent.width
        height: parent.height
        
        AlbumPageComponent {
          id: rightPageAnimated
          
          albumPage: rightPage
          
          width: pageFlipAnimation.width
          height: pageFlipAnimation.height
        }
      }
        
      transform: [
        Rotation {
          id: rotation
          
          origin.x: pageFlipAnimation.width / 2
          origin.y: pageFlipAnimation.height / 2
          
          axis.x: 0
          axis.y: 1
          axis.z: 0
          
          angle: leftToRight ? 0.0 : -180.0
        }
      ]
      
      states: State {
        name: "running"
        PropertyChanges { target: rotation; angle: leftToRight ? -180.0 : 0.0; }
        when: clipper.trigger
      }
      
      transitions: Transition {
        to: "running"
        
        SequentialAnimation {
          ParallelAnimation {
            NumberAnimation {
              target: rotation
              property: "angle"
              duration: durationMsec
              easing.type: Easing.InOutCubic
            }
            
            // halfway through the animation, move the clipping region to the
            // other half of the region
            SequentialAnimation {
              PauseAnimation {
                duration: durationMsec / 2
              }
              ScriptAction {
                script: {
                  if(clipper.x != 0) {
                    clipper.x = 0;
                    flippable.x = 0;
                  } else {
                    clipper.x = pageFlipAnimation.width / 2;
                    flippable.x = -pageFlipAnimation.width / 2;
                  }
                }
              }
            }
          }
          
          // Update album state and change visibility to show the result of
          // the page flip
          ScriptAction {
            script: {
              album.currentPageNumber = finalPageNumber;
              
              if (album.isClosed) {
                leftIsCover = true;
                rightIsCover = true;
              } else {
                leftIsCover = false;
                rightIsCover = false;
                
                if (leftToRight) {
                  leftPage = rightPage;
                } else {
                  rightPage = leftPage;
                }
              }
              
              clipper.visible = false;
            }
          }
          
          // Signal that the page flip is complete
          PropertyAction {
            target: pageFlipAnimation
            property: "pageFlipped"
            value: true
          }
          
          // Reset state for next page flip
          PropertyAction {
            target: clipper
            property: "trigger"
            value: false
          }
        }
      }
    }
  }
}
