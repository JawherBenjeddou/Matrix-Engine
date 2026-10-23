#include "TestFramework.h"

using namespace Matrix;
using namespace Matrix::graphics;

TEST_CASE(Camera, OrthoBoundsFollowZoomAndAspect)
{
    OrthoCamera camera(2.0f, 10.0f);
    CameraBounds bounds = camera.GetBounds();
    CHECK_APPROX(bounds.GetHeight(), 10.0f);
    CHECK_APPROX(bounds.GetWidth(), 20.0f);
    camera.SetZoom(5.0f);
    CHECK_APPROX(camera.GetBounds().GetWidth(), 10.0f);
    camera.SetViewportSize(100, 100);
    CHECK_APPROX(camera.GetBounds().GetWidth(), 5.0f);
    camera.SetZoom(-1.0f); // clamped, never degenerate
    CHECK(camera.GetZoom() > 0.0f);
}

TEST_CASE(Camera, ViewMatrixIsInverseOfCameraTransform)
{
    OrthoCamera camera(1.0f, 10.0f);
    camera.SetPosition(glm::vec3(3.0f, 4.0f, 0.0f));
    glm::vec4 origin = camera.GetViewMatrix() * glm::vec4(3.0f, 4.0f, 0.0f, 1.0f);
    CHECK_APPROX(glm::vec3(origin), glm::vec3(0.0f));
    camera.SetRotation(90.0f);
    glm::vec4 p = camera.GetViewMatrix() * glm::vec4(3.0f, 5.0f, 0.0f, 1.0f); // one unit above the camera
    CHECK(tests::Approx(glm::vec2(p), glm::vec2(1.0f, 0.0f), 1e-4f));
}

TEST_CASE(Camera, ScreenToWorldRoundTrip)
{
    OrthoCamera camera(16.0f / 9.0f, 10.0f);
    camera.SetPosition(glm::vec3(2.0f, -1.0f, 0.0f));
    glm::vec2 viewport(1600.0f, 900.0f);
    CHECK(tests::Approx(camera.ScreenToWorld(viewport * 0.5f, viewport), glm::vec2(2.0f, -1.0f), 1e-3f));
    CHECK(tests::Approx(camera.ScreenToWorld(glm::vec2(0.0f), viewport), glm::vec2(2.0f - 8.8889f, -1.0f + 5.0f), 1e-2f));
    glm::vec2 world(5.0f, 3.0f);
    CHECK(tests::Approx(camera.ScreenToWorld(camera.WorldToScreen(world, viewport), viewport), world, 1e-3f));
}

TEST_CASE(Camera, EditorCameraZoomKeepsCursorFixed)
{
    EditorCamera editor;
    editor.SetViewportSize(1600, 900);
    glm::vec2 mouse(200.0f, 100.0f);
    glm::vec2 before = editor.ScreenToWorld(mouse);
    editor.Zoom(0.5f, &before);
    glm::vec2 after = editor.ScreenToWorld(mouse);
    CHECK(tests::Approx(before, after, 1e-3f));
    editor.Zoom(1000.0f);
    CHECK(editor.GetZoom() <= editor.MaxZoom);
    editor.Reset();
    CHECK_APPROX(editor.GetPosition(), glm::vec2(0.0f));
}

TEST_CASE(Camera, SceneCameraProjection)
{
    SceneCamera camera;
    camera.SetOrthographic(8.0f, -1.0f, 1.0f);
    camera.SetViewportSize(400, 200);
    glm::vec4 corner = camera.GetProjection() * glm::vec4(8.0f, 4.0f, 0.0f, 1.0f);
    CHECK(tests::Approx(glm::vec2(corner), glm::vec2(1.0f, 1.0f), 1e-4f));
    camera.SetViewportSize(0, 0); // ignored
    CHECK_APPROX(camera.GetAspectRatio(), 2.0f);
}

TEST_CASE(Camera, TransformComponentMatrix)
{
    TransformComponent t;
    t.Position = glm::vec3(1.0f, 2.0f, 0.0f);
    t.Rotation = 90.0f;
    t.Scale = glm::vec3(2.0f, 3.0f, 1.0f);
    glm::vec4 p = t.GetTransform() * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    // Scale (2,0) then rotate 90 -> (0,2), then translate -> (1,4)
    CHECK(tests::Approx(glm::vec2(p), glm::vec2(1.0f, 4.0f), 1e-4f));
}
