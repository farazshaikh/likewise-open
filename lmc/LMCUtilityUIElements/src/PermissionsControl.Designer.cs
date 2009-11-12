namespace Likewise.LMC.UtilityUIElements
{
    partial class PermissionsControl
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Component Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.lvGroupOrUserNames = new System.Windows.Forms.ListView();
            this.chIcon = new System.Windows.Forms.ColumnHeader();
            this.chName = new System.Windows.Forms.ColumnHeader();
            this.label1 = new System.Windows.Forms.Label();
            this.btnRemove = new System.Windows.Forms.Button();
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.lblObjectName = new System.Windows.Forms.Label();
            this.lvPermissions = new System.Windows.Forms.ListView();
            this.Permission = new System.Windows.Forms.ColumnHeader();
            this.Allow = new System.Windows.Forms.ColumnHeader();
            this.Deny = new System.Windows.Forms.ColumnHeader();
            this.tbPermissionsFor = new System.Windows.Forms.TextBox();
            this.btnAdd = new System.Windows.Forms.Button();
            this.SuspendLayout();
            //
            // lvGroupOrUserNames
            //
            this.lvGroupOrUserNames.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.chIcon,
            this.chName});
            this.lvGroupOrUserNames.Location = new System.Drawing.Point(15, 58);
            this.lvGroupOrUserNames.Name = "lvGroupOrUserNames";
            this.lvGroupOrUserNames.Size = new System.Drawing.Size(345, 100);
            this.lvGroupOrUserNames.TabIndex = 1;
            this.lvGroupOrUserNames.UseCompatibleStateImageBehavior = false;
            this.lvGroupOrUserNames.View = System.Windows.Forms.View.List;
            //
            // chIcon
            //
            this.chIcon.Text = "";
            //
            // chName
            //
            this.chName.Text = "";
            //
            // label1
            //
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 41);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(108, 13);
            this.label1.TabIndex = 2;
            this.label1.Text = "Group or user names:";
            //
            // btnRemove
            //
            this.btnRemove.Location = new System.Drawing.Point(285, 164);
            this.btnRemove.Name = "btnRemove";
            this.btnRemove.Size = new System.Drawing.Size(75, 23);
            this.btnRemove.TabIndex = 3;
            this.btnRemove.Text = "Remove";
            this.btnRemove.UseVisualStyleBackColor = true;
            //
            // label2
            //
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(12, 169);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(168, 13);
            this.label2.TabIndex = 5;
            this.label2.Text = "To change permissions, click Edit.";
            //
            // label3
            //
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(12, 15);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(70, 13);
            this.label3.TabIndex = 6;
            this.label3.Text = "Object name:";
            //
            // lblObjectName
            //
            this.lblObjectName.AutoSize = true;
            this.lblObjectName.Location = new System.Drawing.Point(88, 15);
            this.lblObjectName.Name = "lblObjectName";
            this.lblObjectName.Size = new System.Drawing.Size(158, 13);
            this.lblObjectName.TabIndex = 7;
            this.lblObjectName.Text = "An Object with Security Settings";
            //
            // lvPermissions
            //
            this.lvPermissions.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.Permission,
            this.Allow,
            this.Deny});
            this.lvPermissions.Location = new System.Drawing.Point(15, 228);
            this.lvPermissions.Name = "lvPermissions";
            this.lvPermissions.Size = new System.Drawing.Size(345, 141);
            this.lvPermissions.TabIndex = 8;
            this.lvPermissions.UseCompatibleStateImageBehavior = false;
            this.lvPermissions.View = System.Windows.Forms.View.List;
            //
            // Permission
            //
            this.Permission.Text = "";
            //
            // Allow
            //
            this.Allow.Text = "";
            //
            // Deny
            //
            this.Deny.Text = "";
            //
            // tbPermissionsFor
            //
            this.tbPermissionsFor.BackColor = System.Drawing.SystemColors.Control;
            this.tbPermissionsFor.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.tbPermissionsFor.Location = new System.Drawing.Point(15, 190);
            this.tbPermissionsFor.Multiline = true;
            this.tbPermissionsFor.Name = "tbPermissionsFor";
            this.tbPermissionsFor.Size = new System.Drawing.Size(252, 32);
            this.tbPermissionsFor.TabIndex = 10;
            this.tbPermissionsFor.Text = "Permissions for Selected Object with Security Settings";
            //
            // btnAdd
            //
            this.btnAdd.Location = new System.Drawing.Point(204, 164);
            this.btnAdd.Name = "btnAdd";
            this.btnAdd.Size = new System.Drawing.Size(75, 23);
            this.btnAdd.TabIndex = 12;
            this.btnAdd.Text = "Add...";
            this.btnAdd.UseVisualStyleBackColor = true;
            //
            // PermissionsControl
            //
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.None;
            this.Controls.Add(this.btnAdd);
            this.Controls.Add(this.tbPermissionsFor);
            this.Controls.Add(this.lvPermissions);
            this.Controls.Add(this.lblObjectName);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.btnRemove);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.lvGroupOrUserNames);
            this.Name = "PermissionsControl";
            this.Size = new System.Drawing.Size(375, 385);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ListView lvGroupOrUserNames;
        private System.Windows.Forms.ColumnHeader chIcon;
        private System.Windows.Forms.ColumnHeader chName;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button btnRemove;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label lblObjectName;
        private System.Windows.Forms.ListView lvPermissions;
        private System.Windows.Forms.TextBox tbPermissionsFor;
        private System.Windows.Forms.ColumnHeader Permission;
        private System.Windows.Forms.ColumnHeader Allow;
        private System.Windows.Forms.ColumnHeader Deny;
        private System.Windows.Forms.Button btnAdd;

    }
}
