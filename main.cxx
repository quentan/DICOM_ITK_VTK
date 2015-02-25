// VTK
#include "vtkSmartPointer.h"
#include "vtkMetaImageReader.h"
#include "vtkImageShiftScale.h"
#include "vtkPiecewiseFunction.h"
#include "vtkColorTransferFunction.h"
#include "vtkVolumeProperty.h"
#include "vtkSmartVolumeMapper.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkVolume.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"

// ITK
#include "itkImageToVTKImageFilter.h"
#include "itkImage.h"
#include "itkImageSeriesReader.h"
#include "itkGDCMImageIO.h"
#include "itkGDCMSeriesFileNames.h"

#define FILE_TYPE 2

int main(int argc, char *argv[]) {

    // S1. Read image
#if FILE_TYPE == 1
    // Meta Image (.mhd .mha)
    vtkSmartPointer<vtkMetaImageReader> image =
            vtkSmartPointer<vtkMetaImageReader>::New();
    image->SetFileName("/Users/Quentan/Develop/VTK/VTKData/Data/HeadMRVolume.mhd");
    image->Update();

#elif FILE_TYPE == 2
    // ITK DICOM image reading
    // S1. Define data types
    const unsigned int kInputDimension = 3;
    const unsigned int kOutputDimension = 2;

    typedef signed short PixelType;

    typedef itk::Image<PixelType, kInputDimension>      InputImageType;
    typedef itk::Image<PixelType, kOutputDimension>     OutputImageType;
    typedef itk::ImageSeriesReader<InputImageType>      ReaderType;
    typedef itk::GDCMImageIO                            ImageIOType;
    typedef itk::GDCMSeriesFileNames                    InputNamesGeneratorType;

    // S2. Define variables using the data type from S1
    // S2.1 Read serial images from directory
    const char *kDirectory =
            "/Users/Quentan/Develop/IMAGE/UCLA_Head_3T_Routine/SUB_MRA";
    InputNamesGeneratorType::Pointer input_names = InputNamesGeneratorType::New();
    input_names->SetInputDirectory(kDirectory);

    const ReaderType::FileNamesContainer & kFilename = input_names->GetInputFileNames();

    ImageIOType::Pointer gdcm_io = ImageIOType::New();
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetImageIO(gdcm_io);
    reader->SetFileNames(kFilename);

    // S2.2. Test whether the reading operation is successful
    try {
        reader->Update();
    }
    catch (itk::ExceptionObject &exception) {
        std::cerr << "Exception thrown while reading the series" << std::endl;
        std::cerr << exception << std::endl;
        return EXIT_FAILURE;
    }

    // S3.1. ITK -> VTK
    typedef itk::ImageToVTKImageFilter<InputImageType> ConnectorType;
    ConnectorType::Pointer connector = ConnectorType::New();
    connector->SetInput(reader->GetOutput());

    // S3.2 Test whether the connection is right
    try {
        connector->Update();
    }
    catch(itk::ExceptionObject &exception) {
        std::cerr << "Exception thrown while connecting ITK and VTK" << std::endl;
        std::cerr << exception << std::endl;
        return EXIT_FAILURE;
    }

    // S4. Deep copy connector's output to an image.
    // If no deepcopy, the connector will go out of scope and vanish, which will cause error while changing slice
    vtkSmartPointer<vtkImageData> image =
            vtkSmartPointer<vtkImageData>::New();
    image->DeepCopy(connector->GetOutput());

#endif // FILE_TYPE

    // S2. Scale volume data to 0-255
    vtkSmartPointer<vtkImageShiftScale> shift_scale =
            vtkSmartPointer<vtkImageShiftScale>::New();
//    shift_scale->SetInputConnection(image->GetOutputPort()); // for Meta image
    shift_scale->SetInputData(image);
    shift_scale->SetOutputScalarTypeToChar();

    // S3.1. Opacity transfer function
    vtkSmartPointer<vtkPiecewiseFunction> opacity_transfer_function =
            vtkSmartPointer<vtkPiecewiseFunction>::New();
    opacity_transfer_function->AddPoint(0, 0.0);
    opacity_transfer_function->AddPoint(100, 0.15);
    opacity_transfer_function->AddPoint(200, 0.15);
    opacity_transfer_function->AddPoint(220, 0.35);

    // S3.2. Colour transfer function
    vtkSmartPointer<vtkColorTransferFunction> colour_transfer_function =
            vtkSmartPointer<vtkColorTransferFunction>::New();
    colour_transfer_function->AddRGBPoint(0, 0.0, 0.0, 0.0);
    colour_transfer_function->AddRGBPoint(100, 1.0, 0.5, 0.3);
    colour_transfer_function->AddRGBPoint(200, 1.0, 0.5, 0.3);
    colour_transfer_function->AddRGBPoint(220, 1.0, 1.0, 0.9);

    // S3.3 Gradient Opacity transfer function
    vtkSmartPointer<vtkPiecewiseFunction> gradient_opacity_transfer_function =
            vtkSmartPointer<vtkPiecewiseFunction>::New();
    gradient_opacity_transfer_function->AddPoint(0, 0.0);
    gradient_opacity_transfer_function->AddPoint(20, 0.5);
    gradient_opacity_transfer_function->AddPoint(30, 1.0);

    // S4. Volume property
    vtkSmartPointer<vtkVolumeProperty> volume_property =
            vtkSmartPointer<vtkVolumeProperty>::New();
    volume_property->SetColor(colour_transfer_function);
    volume_property->SetScalarOpacity(opacity_transfer_function);
    volume_property->SetGradientOpacity(gradient_opacity_transfer_function);
    volume_property->SetInterpolationTypeToLinear();

    volume_property->SetAmbient(0.4);
    volume_property->SetDiffuse(0.6);
    volume_property->SetSpecular(0.2);

    volume_property->ShadeOff();
//    volume_property->ShadeOn();

    // S5. Volume mapper
    vtkSmartPointer<vtkSmartVolumeMapper> smart_volume_mapper =
            vtkSmartPointer<vtkSmartVolumeMapper>::New();
    //!! Note: vtkRenderingVolume should be included in target_link_libraries !!
    smart_volume_mapper->SetRequestedRenderMode(vtkSmartVolumeMapper::DefaultRenderMode);
    smart_volume_mapper->SetInputConnection(shift_scale->GetOutputPort());

    // S6. Set up an Actor
    // The vtkVolume is a vtkProp3D (like a vtkActor) and controls the position
    // and orientation of the volume in world coordinates
    vtkSmartPointer<vtkVolume> volume =
            vtkSmartPointer<vtkVolume>::New();
    volume->SetMapper(smart_volume_mapper);
    volume->SetProperty(volume_property);

    // S7. Set up renderer (the platform), and add the volume to the renderer
    vtkSmartPointer<vtkRenderer> volume_renderer =
            vtkSmartPointer<vtkRenderer>::New();
    volume_renderer->AddViewProp(volume);
    volume_renderer->SetBackground(1.0, 1.0, 1.0);

    // S8. Build up the cinema
    vtkSmartPointer<vtkRenderWindow> render_window =
            vtkSmartPointer<vtkRenderWindow>::New();
    render_window->AddRenderer(volume_renderer);
    render_window->SetSize(640, 480);

    // S9. Set up the interactive mechanic
    vtkSmartPointer<vtkRenderWindowInteractor> render_window_interactor =
            vtkSmartPointer<vtkRenderWindowInteractor>::New();
    render_window_interactor->SetRenderWindow(render_window);

    // S9. Define the interactive method
    vtkSmartPointer<vtkInteractorStyleTrackballCamera> interactor_style =
            vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
    render_window_interactor->SetInteractorStyle(interactor_style);

    // S10. Start rendering
    render_window_interactor->Initialize();
    render_window_interactor->Start();

    return 0;
}