// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may
// not use these files except in compliance with the License. You may obtain
// a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations
// under the License.

#pragma once

#include <Canvas.abi.h>

namespace canvas
{
    using namespace ABI::Windows::Foundation;
    using namespace ABI::Microsoft::Graphics::Canvas;
    using namespace ABI::Windows::UI::Xaml;
    using namespace ABI::Windows::UI::Xaml::Controls;
    using namespace ABI::Windows::UI::Xaml::Media;

    // This helps with cases where we need to explicitly qualify types
    namespace canvasABI = ABI::Microsoft::Graphics::Canvas;

    class CanvasDrawingEventArgsFactory : public ActivationFactory<ICanvasDrawingEventArgsFactory>
    {
        InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_CanvasDrawingEventArgs, BaseTrust);

    public:
        IFACEMETHOD(Create)(
            ICanvasDrawingSession* drawingSession,
            ICanvasDrawingEventArgs** drawEventArgs) override;
    };

    class CanvasDrawingEventArgs : public RuntimeClass<
        ICanvasDrawingEventArgs>
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_CanvasDrawingEventArgs, BaseTrust);

        ClosablePtr<ICanvasDrawingSession> m_drawingSession;

     public:
         CanvasDrawingEventArgs(ICanvasDrawingSession* drawingSession);

         IFACEMETHODIMP get_DrawingSession(ICanvasDrawingSession** value);
    };

    typedef ITypedEventHandler<canvasABI::CanvasControl*, IInspectable*> CreateResourcesEventHandlerType;
    typedef ITypedEventHandler<canvasABI::CanvasControl*, canvasABI::CanvasDrawingEventArgs*> DrawingEventHandlerType;

    class ICanvasControlAdapter
    {
    public:
        virtual std::pair<ComPtr<IInspectable>, ComPtr<IUserControl>> CreateUserControl(IInspectable* canvasControl) = 0;
        virtual ComPtr<ICanvasDevice> CreateCanvasDevice() = 0;
        virtual EventRegistrationToken AddCompositionRenderingCallback(IEventHandler<IInspectable*>*) = 0;
        virtual void RemoveCompositionRenderingCallback(EventRegistrationToken token) = 0;
        virtual ComPtr<ICanvasImageSource> CreateCanvasImageSource(ICanvasDevice* device, int width, int height) = 0;
        virtual ComPtr<IImage> CreateImageControl() = 0;
    };

    class CanvasControl : public RuntimeClass<
        RuntimeClassFlags<WinRtClassicComMix>,
        ICanvasControl,
        ICanvasResourceCreator,
        ABI::Windows::UI::Xaml::IFrameworkElementOverrides,
        ComposableBase<ABI::Windows::UI::Xaml::Controls::IUserControl>>
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_CanvasControl, BaseTrust);

        std::mutex m_drawLock;

        std::shared_ptr<ICanvasControlAdapter> m_adapter;

        EventSource<CreateResourcesEventHandlerType> m_createResourcesEventList;
        EventSource<DrawingEventHandlerType> m_drawEventList;

        EventRegistrationToken m_renderingEventToken;

        ComPtr<ICanvasDevice> m_canvasDevice;
        ComPtr<IImage> m_imageControl;
        ComPtr<ICanvasImageSource> m_canvasImageSource;
        bool m_drawNeeded;
        bool m_isLoaded;

        int m_currentWidth;
        int m_currentHeight;
        
    public:
        CanvasControl(
            std::shared_ptr<ICanvasControlAdapter> adapter);

        //
        // ICanvasControl
        //

        IFACEMETHODIMP add_CreatingResources(
            CreateResourcesEventHandlerType* value,
            EventRegistrationToken *token);

        IFACEMETHODIMP remove_CreatingResources(
            EventRegistrationToken token);

        IFACEMETHODIMP add_Drawing(
            DrawingEventHandlerType* value,
            EventRegistrationToken* token);

        IFACEMETHODIMP remove_Drawing(
            EventRegistrationToken token);

        //
        // ICanvasResourceCreator
        //

        IFACEMETHODIMP get_Device(ICanvasDevice** value);

        IFACEMETHODIMP Invalidate();

        //
        // IFrameworkElementOverrides
        //

        IFACEMETHODIMP MeasureOverride(
            ABI::Windows::Foundation::Size availableSize, 
            ABI::Windows::Foundation::Size* returnValue);

        IFACEMETHODIMP ArrangeOverride(
            ABI::Windows::Foundation::Size finalSize, 
            ABI::Windows::Foundation::Size* returnValue);

        IFACEMETHODIMP OnApplyTemplate();

        HRESULT OnLoaded(IInspectable* sender, IRoutedEventArgs* args);
        HRESULT OnSizeChanged(IInspectable* sender, ISizeChangedEventArgs* args);

    private:
        void CreateBaseClass();
        void CreateImageControl();
        void RegisterEventHandlers();

        void ClearDrawNeeded();
        void EnsureSizeDependentResources();
        void CallDrawHandlers();
        void InvalidateImpl();

        HRESULT OnRenderCallback(IInspectable *pSender, IInspectable *pArgs);        
    };
}
