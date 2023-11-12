# QuickSSH

QuickSSH is a tool designed for IT professionals and developers who need to manage multiple servers efficiently. Say goodbye to the hassle of remembering numerous IP addresses and hostnames. QuickSSH simplifies server management by allowing you to create a list of servers and effortlessly access them.

## Features

- **Server List Management**: Create and maintain a list of your servers.

- **Quick Access**: Connect to any server from your list with a single command, eliminating the need to remember IP addresses or hostnames.

- **Secure Authentication**: QuickSSH supports various authentication methods, including password, SSH keys.

- **SSH Configuration**: QuickSSH integrates with your existing SSH command.

- **Autocomplete**: QuickSSH can integrate with some shells for auto complete commands.

## Getting Started

To connect only use qssh Alias

Commands
  add
      Add a new host to the configuration.
      Options:
          [user]@host Specify the user and host to connect.
          -i Specify the path of the identity file.
          -a Specify a alias to connect to remote server.
          -p Specify the port of remote server.
  remove
      Remove a alias from the configuration.
          Specify the alias of the host to remove.
  list
      List all hosts in the configuration.
  version
      Show version and quit.




## Contributing

We welcome contributions from the community. If you have ideas for improvements or encounter any issues, please open an issue or submit a pull request on our [GitHub repository](https://github.com/josephillips85/qssh).

## Donations

Who want to donate to this project. please doit via Paypal to https://www.paypal.com/paypalme2/josephillips85

## License

QuickSSH is licensed under the GNU License.


Happy server management with QuickSSH!
