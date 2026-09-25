using Godot;
using System;
using System.Runtime.CompilerServices;

public partial class EvelatorDoor : CsgBox3D
{
	[Export] public Vector3 OpenDirection { get; set; } = Vector3.Right;
	private bool m_IsOpen = false;
	private bool m_IsMoving = false;
	private const float m_DoorSpeed = 2.0f;
	private CsgBox3D m_Box;

	private Vector3 m_InitialPosition;
    private Vector3 m_OpenPosition;
	public override void _PhysicsProcess(double delta)
	{
		if (!m_IsMoving)
		{
			return;
		}

		Vector3 targetPosition = m_IsOpen ? m_OpenPosition : m_InitialPosition;
		Vector3 movement = (targetPosition - m_Box.Position).Normalized() * (float)delta * m_DoorSpeed;
		m_Box.Position += movement;
		if ((m_Box.Position - targetPosition).Length() < 0.01f) 
		{
			m_Box.Position = targetPosition; 
			m_IsMoving = false;
        }
    }

	public void ToggleDoor()
	{
		if (m_IsMoving)
		{
			return;
		}
		m_IsOpen = !m_IsOpen;
		m_IsMoving = true;
    }
	
    // Called when the node enters the scene tree for the first time.
    public override void _Ready()
	{
		m_IsMoving = false;
		m_IsOpen = false;
		m_Box = this;
        m_InitialPosition = m_Box.Position;
        m_OpenPosition = m_InitialPosition + OpenDirection.Normalized();
		ToggleDoor();
    }


}
