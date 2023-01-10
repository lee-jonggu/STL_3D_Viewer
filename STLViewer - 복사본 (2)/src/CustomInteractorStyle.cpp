#include "CustomInteractorStyle.h"

CustomInteractorStyle::CustomInteractorStyle()
{
  
}

CustomInteractorStyle::~CustomInteractorStyle()
{

}

void CustomInteractorStyle::OnRightButtonDown()
{
    __super::OnLeftButtonDown();
}

void CustomInteractorStyle::OnRightButtonUp()
{
    __super::OnLeftButtonUp();
}

void CustomInteractorStyle::OnLeftButtonDown()
{

}

void CustomInteractorStyle::OnLeftButtonUp()
{

}

void CustomInteractorStyle::OnMouseWheelForward()
{
    __super::OnMouseWheelForward();
}

void CustomInteractorStyle::OnMouseWheelBackward()
{
    __super::OnMouseWheelBackward();
}