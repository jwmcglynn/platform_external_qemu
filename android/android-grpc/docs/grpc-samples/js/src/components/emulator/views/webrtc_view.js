/*
 * Copyright 2019 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
import PropTypes from "prop-types";
import React, { Component } from "react";
import * as Device from "../../../android_emulation_control/emulator_controller_grpc_web_pb.js";
import JsepProtocolDriver from "../net/JsepProtocolDriver.js"

/**
 * A view on the emulator that is using WebRTC. It will use the Jsep protocol over gRPC to
 * establish the video streams.
 */
export default class EmulatorWebrtcView extends Component {

  constructor() {
    super()
  }

  static propTypes = {
    uri: PropTypes.string, // gRPC endpoint of the emulator
    width: PropTypes.number,
    height: PropTypes.number,
  };

  static defaultProps = {
    width: 1080,
    height: 1920,
  };

  onDisconnect = () => {
    // It is possible that we are not displaying the video component (yet)
    if (this.video)
      this.video.stop()
  }

  onConnect = stream => {
    // This will fire an onLoadedMetadata event, after which
    // we can really start playing the stream.
    this.video.srcObject = stream
  }

  onLoadedMetadata = e => {
      this.video.play().then(_ => {
          console.log("Automatic playback started!")
        })
        .catch(error => {
          // Autoplay is likely disabled in chrome
          // https://developers.google.com/web/updates/2017/09/autoplay-policy-changes
          // so we should probably show something useful here.
          // We explicitly set the video stream to muted, so this shouldn't happen,
          // but is something you will have to fix once enabling audio.
          alert("code: " + error.code + ", msg: " + error.message + ", name: " + error.nane)
          })
  }

  onContextMenu = e => {
    e.preventDefault()
  }

  render() {
    const { width, height, uri } = this.props
    return (
      <div>
        <video ref={node => (this.video = node)}
          width={width}
          height={height}
          muted="muted"
          onContextMenu={this.onContextMenu}
          onLoadedMetadata={this.onLoadedMetadata} />
        <JsepProtocolDriver uri={uri} onConnect={this.onConnect} onDisconnect={this.onDisconnect} />
      </div>
    )
  }
}
