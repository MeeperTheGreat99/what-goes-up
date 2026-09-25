using Godot;
using System;

public partial class Player : RigidBody3D
{
    private Vector2 m_moveInput;

	public override void _Ready()
	{
        m_moveInput = 0.0f;

        Input.MouseMode = Input.MouseModeEnum.Captured;
	}

    public override void _InputEvent(Camera3D camera, InputEvent @event, Vector3 eventPosition, Vector3 normal, int shapeIdx)
    {
        if (@event.GetType() == InputEventMouseMotion)
        {
            InputEventMouseMotion mevent = @event;
        }
    }

    public override void _Process(double delta)
    {
        m_moveInput = 0.0f;
        
        if (Input.IsActionPressed("Forward"))
        {
            m_moveInput.Y += 1.0f;
        }

        if (Input.IsActionPressed("Backward"))
        {
            m_moveInput.Y -= 1.0f;
        }

        if (Input.IsActionPressed("Right"))
        {
            m_moveInput.X += 1.0f;
        }

        if (Input.IsActionPressed("Left"))
        {
            m_moveInput.X -= 1.0f;
        }

        m_moveInput = m_moveInput.Normalized();

        Vector2 mouseInput;
    }

    public override void _PhysicsProcess(double delta)
    {
		
    }
}
