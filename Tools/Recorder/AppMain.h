//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#pragma once

namespace Recorder
{
    class AppMain : public Holographic::AppMainBase
    {
    public:
        AppMain(
            const std::shared_ptr<Graphics::DeviceResources>& deviceResources);

        // IDeviceNotify
        virtual void OnDeviceLost() override;

        virtual void OnDeviceRestored() override;

        // IAppMain
        virtual void OnHolographicSpaceChanged(
            _In_ Windows::Graphics::Holographic::HolographicSpace^ holographicSpace) override;

        virtual void OnSpatialInput(
            _In_ Windows::UI::Input::Spatial::SpatialInteractionSourceState^ pointerState) override;

        virtual void OnUpdate(
            _In_ Windows::Graphics::Holographic::HolographicFrame^ holographicFrame,
            _In_ const Graphics::StepTimer& stepTimer) override;

        virtual void OnPreRender() override;

        virtual void OnRender() override;

    private:
        // Start and stop recording frames.
        void StartRecording();
        void StopRecording();

        // Initializes a speech recognizer.
        bool InitializeSpeechRecognizer();

        // Creates a speech command recognizer, and starts listening.
        concurrency::task<bool> StartRecognizeSpeechCommands();

        // Resets the speech recognizer, if one exists.
        concurrency::task<void> StopCurrentRecognizerIfExists();

        // Say a sentence using the speech synthesizer.
        void SaySentence(Platform::StringReference sentence);

        // Process continuous speech recognition results.
        void OnResultGenerated(
            Windows::Media::SpeechRecognition::SpeechContinuousRecognitionSession ^sender,
            Windows::Media::SpeechRecognition::SpeechContinuousRecognitionResultGeneratedEventArgs ^args);

        // Recognize when conditions might impact speech recognition quality.
        void OnSpeechQualityDegraded(
            Windows::Media::SpeechRecognition::SpeechRecognizer^ recognizer,
            Windows::Media::SpeechRecognition::SpeechRecognitionQualityDegradingEventArgs^ args);

        // Plays the voice command recognition sound.
        void PlayVoiceCommandRecognitionSound();

        // Initializes access to HoloLens sensors.
        void StartHoloLensMediaFrameSourceGroup();

    private:
        // Handles playback of the voice UI prompt.
        OmnidirectionalSound _speechSynthesisSound;
        OmnidirectionalSound _voiceCommandRecognitionSound;

        // Handles the start/stop commands for recording.
        Windows::Media::SpeechRecognition::SpeechRecognizer^ _speechRecognizer;

        // Handles the speech feedback to the user.
        Windows::Media::SpeechSynthesis::SpeechSynthesizer^ _speechSynthesizer;

        Windows::Foundation::EventRegistrationToken _speechRecognizerResultEventToken;
        Windows::Foundation::EventRegistrationToken _speechRecognitionQualityDegradedToken;

        //
        // HoloLens media frame source groups -- one for the HoloLens Research Mode
        // sensor streaming support and one for the HoloLens Photo-Video camera.
        //
        HoloLensForCV::MediaFrameSourceGroup^ _mediaFrameSourceGroup;
        std::atomic_bool _mediaFrameSourceGroupStarted;

        HoloLensForCV::SensorFrameRecorder^ _sensorFrameRecorder;
        std::atomic_bool _sensorFrameRecorderStarted;

        std::mutex _startStopRecordingMutex;

        // Camera preview.
        std::unique_ptr<Rendering::SlateRenderer> _slateRenderer;
        Rendering::Texture2DPtr _cameraPreviewTexture;
        Windows::Foundation::DateTime _cameraPreviewTimestamp;

        // Default is 0. 0 is equivalent to manual stop mode.
        unsigned int _maxTimeToRecordInSeconds;
        dbg::Timer _recordTimer;
    };
}
