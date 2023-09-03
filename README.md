<p align="center">
  <img src="img/logo.svg" alt="Tau Logo" width="80%">
</p>

<h2 align="center">A unified language for efficient and expressive code.</h2>

Welcome to Tau, a powerful and versatile programming language for building
efficient and reliable software. It seamlessly combines low-level capabilities
with high-level concepts to empower developers all round the world.

## 📚 Documentation

For comprehensive information about the Tau programming language, including
language specifications, syntax, and usage examples, please visit our
[official website](https://docs.tau-lang.org/).

- [**Language Reference**](https://docs.tau-lang.org/language-reference) -
  Explore the core concepts and features of Tau.
- [**Getting Started**](https://docs.tau-lang.org/getting-started) -
  Learn how to set up and begin using Tau for your projects.
- [**Tutorials**](https://docs.tau-lang.org/tutorials) -
  Dive into practical tutorials to enhance your Tau skills.
- [**API Reference**](https://docs.tau-lang.org/api-reference) -
  If you're interested in extending Tau, check out the API reference.

## ⚙️ Installation

You can easily install Tau on various platforms using the method that suits your
environment best. Choose from the options below:

### Windows

On Windows, we recommend using the **Tau Installer** for a hassle-free
installation experience. Download the installer from our
[website](https://tau-lang.org/downloads) and follow the on-screen instructions.

You can also install Tau using [🍫 Chocolatey](https://chocolatey.org/).

Once Chocolatey is installed, run the following command to install Tau:

```sh
choco install tau
```

### macOS

For macOS users, you can install Tau via the popular package manager **Homebrew**.
If you haven't already installed Homebrew, you can do so by following the instructions
on the [🍺 Homebrew website](https://brew.sh/).

Once Homebrew is installed, run the following command to install Tau:

```sh
brew install tau
```

### Linux

On Linux distributions, we provide packages for various package managers. Choose
the package manager that matches your distribution:

#### Debian/Ubuntu

```sh
sudo apt-get install tau
```

#### Fedora

```sh
sudo dnf install tau
```

#### Arch Linux (via AUR)

```sh
yay -S tau
```

#### Other Distributions

Please refer to our [installation guide](https://tau-lang.org/install) for
instructions tailored to your specific Linux distribution.

After installation, you can verify that Tau is correctly installed by running:

```sh
tau --version
```

This will display the installed Tau version.

👏 That's it! You're now ready to start using Tau for your programming projects.
If you encounter any issues during installation or have questions, feel free to
seek assistance from our [community](https://tau-lang.org/community).

❤️ Happy coding with Tau!

## 📦 Usage

Here's how you can create and compile your first Tau program.

Create a file called `my_program.tau` and add the following code:

```tau
use std.io

fun main(): i32 {
  stdout.write("Hello World!")
  return 0
}
```

Once that's done, you can compile the program by typing:

```sh
tau build my_program.tau -o my_program
```

And to run it:

```sh
./my_program
```

If everything went according to plan, you should see the following printed to
your screen:

```text
Hello World!
```

## 🤝 Contributing

We welcome contributions from the community! If you'd like to contribute to the
Tau project, please check out our [Contribution Guidelines](CONTRIBUTING.md).

## 📄 License

This project is licensed under the [Apache 2.0 License](LICENSE).
