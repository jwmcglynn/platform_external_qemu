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
import EmulatorPngView from "./simple_png_view.js"
import JsepProtocolDriver from "../net/JsepProtocolDriver.js"

/**
 * A View that will use WebRTC if possible, and otherwise will revert to
 * using the png view.
 */
export default class FallbackView extends Component {
  static propTypes = {
    uri: PropTypes.string, // gRPC endpoint of the emulator
    width: PropTypes.number,
    height: PropTypes.number,
    refreshRate: PropTypes.number
  };

  static defaultProps = {
    width: 1080,
    height: 1920,
    refreshRate: 1
  };

  state = {
      fallback: true,
  }

  onDisconnect = () => {
    this.setState({fallback: true})
  }

  onConnect = stream => {
    this.setState({ fallback: false }, () => {
      this.video.srcObject = stream
    })
  }

  onLoadedMetadata = e => {
    this.video.play().catch(error => {
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
    const { width, height, uri, refreshRate } = this.props;
    const { fallback } = this.state
    return (
        <div>
            <JsepProtocolDriver uri={uri} onConnect={this.onConnect} onDisconnect={this.onDisconnect}/>
            { !fallback && <video ref={node => (this.video = node)}
                                  width={width}
                                  height={height}
                                  onContextMenu={this.onContextMenu}
                                  onLoadedMetadata={this.onLoadedMetadata}
                                  muted="muted"/> }
            { fallback && <EmulatorPngView uri={uri} refreshRate={refreshRate} width={width} height={height} /> }
        </div>
    )
  }
}
