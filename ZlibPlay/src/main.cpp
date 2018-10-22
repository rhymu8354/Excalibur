/**
 * @file main.cpp
 *
 * This module holds the main() function, which is the entrypoint
 * to the program.
 *
 * © 2018 by Richard Walters
 */

#include <functional>
#include <inttypes.h>
#include <memory>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <zlib.h>

namespace {

    /**
     * This is the number of bytes that we will allocate at a time while
     * deflating data.
     */
    constexpr size_t DEFLATE_BUFFER_INCREMENT = 256;

    /**
     * This is the number of bytes that we will allocate for the buffer used to
     * receive inflated data.
     */
    constexpr size_t INFLATE_BUFFER_SIZE = 256;

    /**
     * This function prints to the standard error stream information
     * about how to use this program.
     */
    void PrintUsageInformation() {
        fprintf(
            stderr,
            (
                "Usage: ZlibPlay\n"
                "\n"
                "Do stuff with zlib.\n"
            )
        );
    }

    /**
     * This contains variables set through the operating system environment
     * or the command-line arguments.
     */
    struct Environment {
    };

    /**
     * This function updates the program environment to incorporate
     * any applicable command-line arguments.
     *
     * @param[in] argc
     *     This is the number of command-line arguments given to the program.
     *
     * @param[in] argv
     *     This is the array of command-line arguments given to the program.
     *
     * @param[in,out] environment
     *     This is the environment to update.
     *
     * @param[in] diagnosticMessageDelegate
     *     This is the function to call to publish any diagnostic messages.
     *
     * @return
     *     An indication of whether or not the function succeeded is returned.
     */
    bool ProcessCommandLineArguments(
        int argc,
        char* argv[],
        Environment& environment
    ) {
        enum class State {
            // First command-line argument
            Initial,
        } state = State::Initial;
        for (int i = 1; i < argc; ++i) {
            const std::string arg(argv[i]);
            switch (state) {
                case State::Initial: { // next argument
                    state = State::Initial;
                } break;
            }
        }
        return true;
    }

    /**
     * This function writes the given data to the given file,
     * erasing the file's previous contents.
     */
    void DumpFile(
        const std::string& fileName,
        const std::vector< uint8_t >& fileContent
    ) {
        const auto fileHandle = std::unique_ptr< FILE, std::function< void(FILE*) > >(
            fopen(fileName.c_str(), "wb"),
            [](FILE*f){
                if (f != NULL) {
                    (void)fclose(f);
                }
            }
        );
        if (fileHandle == NULL) {
            return;
        }
        (void)fwrite(fileContent.data(), fileContent.size(), 1, fileHandle.get());
    }

}

/**
 * This function does stuff with the inflate/deflate functions from zlib.
 *
 * @param[in] environment
 *     This contains variables set through the operating system environment
 *     or the command-line arguments.
 */
void PlayWithInflateDeflate(const Environment& environment) {
    printf("\nPlaying with deflate:\n-----------------------------\n");

    // This is the input to the process: the string to deflate and then
    // inflate.
    const std::string input = "Hello, World!";
    // const std::string input = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\"> <html> <head> <meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"> <title>The LLVM Compiler Infrastructure Project</title> <link rel=\"stylesheet\" href=\"/llvm.css\" type=\"text/css\"> </head> <body style=\"margin-left:0;\"> <div class=\"www_title\"> The <strong>LLVM</strong> Compiler Infrastructure </div> <table width=\"100%\" border=0 cellpadding=10> <tr> <td valign=top align=center> <!-- <img src=\"img/DragonSmall.png\" width=\"136\" height=\"136\"> --> <b>Site Map:</b> <div class=\"www_sidebar\"> <a href=\"/\">Overview</a><br> <a href=\"/Features.html\">Features</a><br> <a href=\"/docs/\">Documentation</a><br> <a href=\"/docs/CommandGuide/\">Command Guide</a><br> <a href=\"/docs/FAQ.html\">FAQ</a><br> <a href=\"/pubs/\">Publications</a><br> <a href=\"/ProjectsWithLLVM/\">LLVM&nbsp;Projects</a><br> <a href=\"/OpenProjects.html\">Open&nbsp;Projects</a><br> <a href=\"/Users.html\">LLVM&nbsp;Users</a><br> <a href=\"http://bugs.llvm.org/\">Bug&nbsp;Database</a><br> <a href=\"/Logo.html\">LLVM Logo</a><br> <a href=\"http://blog.llvm.org/\">Blog</a><br> <a href=\"/devmtg/\">Meetings</a><br> <a href=\"/foundation/\">LLVM&nbsp;Foundation</a><br> </div> <br> <b>Download!</b> <div class=\"www_sidebar\"> <span style=\"font-size:smaller\">Download now:</span> <a href=\"/releases/download.html#7.0.0\"><b>LLVM&nbsp;7.0.0</b></a><br> <a href=\"/releases/\">All Releases</a><br> <a href=\"https://apt.llvm.org/\">APT Packages</a><br> <a href=\"/builds/\">Win Installer</a><br> <br> <span style=\"font-size:smaller\">View the open-source</span><br> <a href=\"/releases/7.0.0/LICENSE.TXT\"><b>license</b></a> </div> <br> <b>Search&nbsp;this&nbsp;Site</b> <div class=\"www_sidebar\"> <form action=\"https://www.google.com/search\" method=\"get\"> <div> <input type=\"hidden\" name=\"sitesearch\" value=\"llvm.org\"> <input type=text name=q size=11><br> <input type=submit value=\"Search!\" name=\"submit\"> </div> </form> </div> <br> <b>Useful&nbsp;Links</b> <div class=\"www_sidebar\"> <span style=\"font-size:smaller\">Mailing Lists:</span><br> <a href=\"http://lists.llvm.org/mailman/listinfo/llvm-announce\">LLVM-announce</a><br> <a href=\"http://lists.llvm.org/mailman/listinfo/llvm-dev\">LLVM-dev</a><br> <a href=\"http://lists.llvm.org/mailman/listinfo/llvm-bugs\">LLVM-bugs</a><br> <a href=\"http://lists.llvm.org/mailman/listinfo/llvm-commits\">LLVM-commits</a><br> <a href=\"http://lists.llvm.org/mailman/listinfo/llvm-branch-commits\">LLVM-branch-commits</a><br> <a href=\"http://lists.llvm.org/mailman/listinfo/llvm-testresults\">LLVM-testresults</a> <br><br> <span style=\"font-size:smaller\">IRC Channel:</span><br> <a href=\"/docs/#irc\">irc.oftc.net #llvm</a> <br><br> <span style=\"font-size:smaller\">Dev. Resources:</span><br> <a href=\"/doxygen/\">doxygen</a> <a href=\"/viewvc/llvm-project/\">ViewVC</a><br> <a href=\"http://blog.llvm.org/\">Blog</a> <a href=\"http://bugs.llvm.org/\">Bugzilla</a><br> <a href=\"http://lab.llvm.org:8011/\">Buildbot</a> <a href=\"http://green.lab.llvm.org/green/\">Green Dragon</a> <a href=\"http://lnt.llvm.org/\">LNT</a> <a href=\"/reports/coverage/\">Coverage</a> <a href=\"/reports/scan-build/\">Scan-build</a> <a href=\"http://lab.llvm.org:8080/coverage/coverage-reports/index.html\">llvm-cov</a> </div> <br> <b>Release&nbsp;Emails</b> <div class=\"www_sidebar\"> <span style=\"font-size:smaller\"> 7.0.0: <a href=\"http://lists.llvm.org/pipermail/llvm-announce/2018-September/000080.html\">Sep 2018</a><br> 6.0.1: <a href=\"http://lists.llvm.org/pipermail/llvm-announce/2018-July/000079.html\">July 2018</a><br> 5.0.2: <a href=\"http://lists.llvm.org/pipermail/llvm-announce/2018-May/000078.html\">May 2018</a><br> 6.0.0: <a href=\"http://lists.llvm.org/pipermail/llvm-announce/2018-March/000077.html\">Mar 2018</a><br> 5.0.1: <a href=\"http://lists.llvm.org/pipermail/llvm-announce/2017-December/000076.html\">Dec 2017</a><br> 5.0.0: <a href=\"http://lists.llvm.org/pipermail/llvm-announce/2017-September/000075.html\">Sep 2017</a><br> 4.0.1: <a href=\"http://lists.llvm.org/pipermail/llvm-announce/2017-July/000074.html\">Jul 2017</a><br> 4.0.0: <a href=\"http://lists.llvm.org/pipermail/llvm-announce/2017-March/000073.html\">Mar 2017</a><br> 3.9.1: <a href=\"http://lists.llvm.org/pipermail/llvm-announce/2016-December/000072.html\">Dec 2016</a><br> 3.9.0: <a href=\"http://lists.llvm.org/pipermail/llvm-announce/2016-September/000070.html\">Sep 2016</a><br> 3.8.1: <a href=\"http://lists.llvm.org/pipermail/llvm-announce/2016-July/000068.html\">July 2016</a><br> 3.8.0: <a href=\"http://lists.llvm.org/pipermail/llvm-announce/2016-March/000067.html\">Mar 2016</a><br> 3.7.1: <a href=\"http://lists.llvm.org/pipermail/llvm-announce/2016-January/000066.html\">Jan 2016</a><br> 3.7.0: <a href=\"http://lists.llvm.org/pipermail/llvm-announce/2015-September/000065.html\">Sep 2015</a><br> 3.6.2: <a href=\"http://lists.llvm.org/pipermail/llvm-announce/2015-July/000061.html\">Jul 2015</a><br> 3.6.1: <a href=\"http://lists.llvm.org/pipermail/llvm-announce/2015-May/000059.html\">May 2015</a><br> 3.5.2: <a href=\"http://lists.llvm.org/pipermail/llvm-announce/2015-April/000058.html\">Apr 2015</a><br> 3.6: <a href=\"http://lists.llvm.org/pipermail/llvm-announce/2015-February/000057.html\">Feb 2015</a><br> 3.5.1: <a href=\"http://lists.llvm.org/pipermail/llvm-announce/2015-January/000056.html\">Jan 2015</a><br> 3.5: <a href=\"http://lists.llvm.org/pipermail/llvm-announce/2014-September/000055.html\">Sep 2014</a><br> 3.4.2: <a href=\"http://lists.llvm.org/pipermail/llvm-announce/2014-June/000053.html\">June 2014</a><br> 3.4.1: <a href=\"http://lists.llvm.org/pipermail/llvm-announce/2014-May/000052.html\">May 2014</a><br> 3.4: <a href=\"http://lists.llvm.org/pipermail/llvm-announce/2014-January/000049.html\">Jan 2014</a><br> 3.3: <a href=\"http://lists.llvm.org/pipermail/llvm-announce/2013-June/000046.html\">Jun 2013</a><br> 3.2: <a href=\"http://lists.llvm.org/pipermail/llvm-announce/2012-December/000043.html\">Dec 2012</a><br> 3.1: <a href=\"http://lists.llvm.org/pipermail/llvm-announce/2012-May/000041.html\">May 2012</a><br> 3.0: <a href=\"http://lists.llvm.org/pipermail/llvm-announce/2011-December/000039.html\">Dec 2011</a><br> 2.9: <a href=\"http://lists.llvm.org/pipermail/llvm-announce/2011-April/000037.html\">Apr 2011</a><br> 2.8: <a href=\"http://lists.llvm.org/pipermail/llvm-announce/2010-October/000036.html\">Oct  2010</a><br> 2.7: <a href=\"http://lists.llvm.org/pipermail/llvm-announce/2010-April/000034.html\">Apr 2010</a><br> 2.6: <a href=\"http://lists.llvm.org/pipermail/llvm-announce/2009-October/000033.html\">Oct 2009</a><br> 2.5: <a href=\"http://lists.llvm.org/pipermail/llvm-announce/2009-March/000031.html\">Mar 2009</a><br> 2.4: <a href=\"http://lists.llvm.org/pipermail/llvm-announce/2008-November/000030.html\">Nov 2008</a><br> 2.3: <a href=\"http://lists.llvm.org/pipermail/llvm-announce/2008-June/000027.html\">Jun 2008</a><br> 2.2: <a href=\"http://lists.llvm.org/pipermail/llvm-announce/2008-February/000025.html\">Feb 2008</a><br> 2.1: <a href=\"http://lists.llvm.org/pipermail/llvm-announce/2007-September/000024.html\">Sep 2007</a><br> 2.0: <a href=\"http://lists.llvm.org/pipermail/llvm-announce/2007-May/000023.html\">May 2007</a><br> <a href=\"/OldNews.html\">Older News</a><br> </span> </div> <br> <span style=\"font-size:smaller\">Maintained by the <br> <a href=\"mailto:llvm-admin@lists.llvm.org\">llvm-admin team</a></span> </td> <td valign=top> <table border=\"0\" cellpadding=\"0\" cellspacing=\"0\" width=\"95%\"> <!-- Start of the left bar... --> <tr> <td valign=\"top\" style=\"width:64%\"> <div class=\"www_sectiontitle\">LLVM Overview</div> <div class=\"www_text\"> <p>The LLVM Project is a collection of modular and reusable compiler and toolchain technologies.  Despite its name, LLVM has little to do with traditional virtual machines.  The name \"LLVM\" itself is not an acronym; it is the full name of the project.</p> <p>LLVM began as a <a href=\"pubs/2004-01-30-CGO-LLVM.html\">research project</a> at the <a href=\"http://cs.illinois.edu/\">University of Illinois</a>, with the goal of providing a modern, SSA-based compilation strategy capable of supporting both static and dynamic compilation of arbitrary programming languages.  Since then, LLVM has grown to be an umbrella project consisting of a number of subprojects, many of which are being used in production by a wide variety of <a href=\"Users.html\">commercial and open source</a> projects as well as being widely used in <a href=\"pubs/\">academic research</a>.  Code in the LLVM project is licensed under the <a href=\"docs/DeveloperPolicy.html#license\">\"UIUC\" BSD-Style license</a>.</p> <p>The primary sub-projects of LLVM are:</p> <ol> <li><p>The <b>LLVM Core</b> libraries provide a modern source- and target-independent <a href=\"docs/Passes.html\">optimizer</a>, along with <a href=\"docs/CodeGenerator.html\">code generation support</a> for many popular CPUs (as well as some less common ones!) These libraries are built around a <a href=\"docs/LangRef.html\">well specified</a> code representation known as the LLVM intermediate representation (\"LLVM IR\").  The LLVM Core libraries are <a href=\"docs/\">well documented</a>, and it is particularly easy to invent your own language (or port an existing compiler) to use <a href=\"docs/tutorial/\">LLVM as an optimizer and code generator</a>.</p> </li> <li><p><b><a href=\"http://clang.llvm.org\">Clang</a></b> is an \"LLVM native\" C/C++/Objective-C compiler, which aims to deliver amazingly fast compiles (e.g. about <a href=\"http://clang.llvm.org/features.html#performance\">3x faster than GCC</a> when compiling Objective-C code in a debug configuration), extremely useful <a href=\"http://clang.llvm.org/diagnostics.html\">error and warning messages</a> and to provide a platform"
    // " for building great source level tools.  The <a href=\"http://clang-analyzer.llvm.org/\">Clang Static Analyzer</a> is a tool that automatically finds bugs in your code, and is a great example of the sort of tool that can be built using the Clang frontend as a library to parse C/C++ code.</p></li> <li><p>The <b><a href=\"http://lldb.llvm.org\">LLDB</a></b> project builds on libraries provided by LLVM and Clang to provide a great native debugger. It uses the Clang ASTs and expression parser, LLVM JIT, LLVM disassembler, etc so that it provides an experience that \"just works\".  It is also blazing fast and much more memory efficient than GDB at loading symbols. </p></li> <li><p>The <b><a href=\"http://libcxx.llvm.org\">libc++</a></b> and <b><a href=\"http://libcxxabi.llvm.org\">libc++ ABI</a></b> projects provide a standard conformant and high-performance implementation of the C++ Standard Library, including full support for C++11 and C++14.</p></li> <li><p>The <b><a href=\"http://compiler-rt.llvm.org\">compiler-rt</a></b> project provides highly tuned implementations of the low-level code generator support routines like \"<tt>__fixunsdfdi</tt>\" and other calls generated when a target doesn't have a short sequence of native instructions to implement a core IR operation. It also provides implementations of run-time libraries for dynamic testing tools such as <a href=\"http://clang.llvm.org/docs/AddressSanitizer.html\">AddressSanitizer</a>, <a href=\"http://clang.llvm.org/docs/ThreadSanitizer.html\">ThreadSanitizer</a>, <a href=\"http://clang.llvm.org/docs/MemorySanitizer.html\">MemorySanitizer</a>, and <a href=\"http://clang.llvm.org/docs/DataFlowSanitizer.html\">DataFlowSanitizer</a>. </p></li> <li><p>The <b><a href=\"http://openmp.llvm.org\">OpenMP</a></b> subproject provides an <a href=\"http://openmp.org\">OpenMP</a> runtime for use with the OpenMP implementation in Clang.</p></li> <li><p>The <b><a href=\"http://polly.llvm.org/\">polly</a></b> project implements a suite of cache-locality optimizations as well as auto-parallelism and vectorization using a polyhedral model.</p></li> <li><p>The <b><a href=\"http://libclc.llvm.org/\">libclc</a></b> project aims to implement the OpenCL standard library.</p></li> <li><p>The <b><a href=\"http://klee.llvm.org\">klee</a></b> project implements a \"symbolic virtual machine\" which uses a theorem prover to try to evaluate all dynamic paths through a program in an effort to find bugs and to prove properties of functions.  A major feature of klee is that it can produce a testcase in the event that it detects a bug.</p></li> <li><p>The <b><a href=\"http://safecode.cs.illinois.edu\">SAFECode</a></b> project is a memory safety compiler for C/C++ programs.  It instruments code with run-time checks to detect memory safety errors (e.g., buffer overflows) at run-time.  It can be used to protect software from security attacks and can also be used as a memory safety error debugging tool like Valgrind.</p></li> <li><p>The <b><a href=\"http://lld.llvm.org/\">LLD</a></b> project is a new linker. That is a drop-in replacement for system linkers and runs much faster.</p></li> </ol> <p>In addition to official subprojects of LLVM, there are a broad variety of other projects that <a href=\"/ProjectsWithLLVM/\">use components of LLVM for various tasks</a>.  Through these external projects you can use LLVM to compile Ruby, Python, Haskell, Java, D, PHP, Pure, Lua, and a number of other languages.  A major strength of LLVM is its versatility, flexibility, and reusability, which is why it is being used for such a wide variety of different tasks: everything from doing light-weight JIT compiles of embedded languages like Lua to compiling Fortran code for massive super computers.</p> <p>As much as everything else, LLVM has a broad and friendly community of people who are interested in building great low-level tools.  If you are interested in getting involved, a good first place is to skim the <a href=\"http://blog.llvm.org\">LLVM Blog</a> and to sign up for the <a href=\"http://lists.llvm.org/mailman/listinfo/llvm-dev\">LLVM Developer mailing list</a>.  For information on how to send in a patch, get commit access, and copyright and license topics, please see <a href=\"docs/DeveloperPolicy.html\">the LLVM Developer Policy</a>. </p> </div> </td> <!-- Dividing space between columns --> <td style=\"width:2%\"></td> <!-- Start of the right bar... 359 --> <td valign=\"top\" style=\"width:35%\"> <div class=\"www_sectiontitle\">Latest LLVM Release!</div> <div class=\"www_text\"> <p><b>19 September 2018</b>: LLVM 7.0.0 is now <a href=\"releases/\"><b>available for download</b></a>!  LLVM is publicly available under an open source <a href=\"releases/7.0.0/LICENSE.TXT\">License</a>.  Also, you might want to check out <a href=\"docs/ReleaseNotes.html#whatsnew\"><b>the new features</b></a> in SVN that will appear in the next LLVM release.  If you want them early, <a href=\"releases/\">download LLVM</a> through anonymous SVN.</p> </div> <div class=\"www_sectiontitle\">ACM Software System Award!</div> <div class=\"www_text\"> <p>LLVM has been awarded the <b>2012 ACM Software System Award</b>! This award is given by ACM to <i>one</i> software system worldwide every year. <!-- and is \"<i>Awarded to an institution or individual(s) recognized for developing a software system that has had a lasting influence, reflected in contributions to concepts, in commercial acceptance, or both</i>\".--> LLVM is <a href=\"http://awards.acm.org/software_system/year.cfm\"> in highly distinguished company</a>! Click on any of the individual recipients' names on that page for the detailed citation describing the award. </p> </div> <div class=\"www_sectiontitle\">Upcoming Releases</div> <div class=\"www_text\"> <p><b>LLVM Release Schedule:</b></p> <ul> <li>7.0.1: <ul> <li>To be decided.</li> </ul> </li> </ul> </div> <div class=\"www_sectiontitle\">Developer&nbsp;Meetings</div> <div class=\"www_text\"> <p>Upcoming: <a href=\"/devmtg/2018-10/\">October 17-18, 2018 (Workshop October 16)</a></p> <p>Proceedings from past meetings:</p> <ul> <li><a href=\"/devmtg/2018-04/\">April 16-17, 2018</a></li> <li><a href=\"/devmtg/2017-10/\">October 18-19, 2017</a></li> <li><a href=\"/devmtg/2017-03/\">March 27-28, 2017</a></li> <li><a href=\"/devmtg/2016-11/\">November 3-4, 2016</a></li> <li><a href=\"/devmtg/2016-03/\">March 17-18, 2016</a></li> <li><a href=\"/devmtg/2015-10/\">October 29-30, 2015</a></li> <li><a href=\"/devmtg/2015-04/\">April 13-14, 2015</a></li> <li><a href=\"/devmtg/2014-10/\">October 28-29, 2014</a></li> <li><a href=\"/devmtg/2014-04/\">April 7-8, 2014</a></li> <li><a href=\"/devmtg/2013-11/\">Nov 6-7, 2013</a></li> <li><a href=\"/devmtg/2013-04/\">April 29-30, 2013</a></li> <li><a href=\"/devmtg/2012-11/\">November 7-8, 2012</a></li> <li><a href=\"/devmtg/2012-04-12/\">April 12, 2012</a></li> <li><a href=\"/devmtg/2011-11/\">November 18, 2011</a></li> <li><a href=\"/devmtg/2011-09-16/\">September 2011</a></li> <li><a href=\"/devmtg/2010-11/\">November 2010</a></li> <li><a href=\"/devmtg/2009-10/\">October 2009</a></li> <li><a href=\"/devmtg/2008-08/\">August 2008</a></li> <li><a href=\"/devmtg/2007-05/\">May 2007</a></li> </ul> </div> <br> <!-- End of the right column --> </td> </tr> </table> </td></tr> </table> </body> </html> ";

    // Here's the buffer in which we'll put the deflated data.
    std::vector< uint8_t > deflatedContent;

    // Here's the buffer in which we'll put the inflated data.
    std::vector< uint8_t > inflatedContent(INFLATE_BUFFER_SIZE);

    // This is where we'll build up the inflated data output.
    std::string output;

    // Initialize the deflate stream.
    z_stream deflateStream;
    deflateStream.zalloc = Z_NULL;
    deflateStream.zfree = Z_NULL;
    deflateStream.opaque = Z_NULL;
    if (deflateInit(&deflateStream, Z_DEFAULT_COMPRESSION) == Z_OK) {
        printf("deflateInit succeeded.\n");
    } else {
        printf("deflateInit failed.\n");
        return;
    }

    // Deflate the data.
    deflateStream.next_in = (Bytef*)input.c_str();
    deflateStream.avail_in = (uInt)input.length();
    deflateStream.total_in = 0;
    int result = Z_OK;
    while (result != Z_STREAM_END) {
        size_t totalDeflatedPreviously = deflatedContent.size();
        deflatedContent.resize(totalDeflatedPreviously + DEFLATE_BUFFER_INCREMENT);
        deflateStream.next_out = (Bytef*)deflatedContent.data() + totalDeflatedPreviously;
        deflateStream.avail_out = DEFLATE_BUFFER_INCREMENT;
        deflateStream.total_out = 0;
        result = deflate(&deflateStream, Z_FINISH);
        deflatedContent.resize(totalDeflatedPreviously + (size_t)deflateStream.total_out);
        if (result == Z_OK) {
            printf("deflate produced %zu more bytes.\n", (size_t)deflateStream.total_out);
        } else if (result == Z_STREAM_END) {
            printf("deflate produced last %zu bytes.\n", (size_t)deflateStream.total_out);
        } else if (result != Z_BUF_ERROR) {
            printf("deflate failed (%d: %s).\n", (int)result, deflateStream.msg);
            return;
        }
    }

    // Display deflated data.
    printf("Deflated data:");
    for (size_t i = 0; i < deflatedContent.size(); ++i) {
        if ((i % 16) == 0) {
            printf("\n");
        } else {
            printf(" ");
        }
        printf("%02" PRIX8, deflatedContent[i]);
    }
    printf("\n");

    // End the deflate stream.
    result = deflateEnd(&deflateStream);
    if (result == Z_OK) {
        printf("deflateEnd succeeded.\n");
    } else {
        printf("deflateEnd failed (%d: %s).\n", (int)result, deflateStream.msg);
        return;
    }

    // Initialize the inflate stream.
    z_stream inflateStream;
    inflateStream.zalloc = Z_NULL;
    inflateStream.zfree = Z_NULL;
    inflateStream.opaque = Z_NULL;
    if (inflateInit(&inflateStream) == Z_OK) {
        printf("inflateInit succeeded.\n");
    } else {
        printf("inflateInit failed.\n");
        return;
    }

    // inflate the data.
    inflateStream.next_in = (Bytef*)deflatedContent.data();
    inflateStream.avail_in = (uInt)deflatedContent.size();
    inflateStream.total_in = 0;
    result = Z_OK;
    while (result != Z_STREAM_END) {
        size_t totalInflatedPreviously = inflatedContent.size();
        inflateStream.next_out = (Bytef*)inflatedContent.data();
        inflateStream.avail_out = (uInt)inflatedContent.size();
        inflateStream.total_out = 0;
        result = inflate(&inflateStream, Z_FINISH);
        output += std::string(
            inflatedContent.begin(),
            inflatedContent.begin() + (size_t)inflateStream.total_out
        );
        if (result == Z_OK) {
            printf("inflate produced %zu more bytes.\n", (size_t)inflateStream.total_out);
        } else if (result == Z_STREAM_END) {
            printf("inflate produced last %zu bytes.\n", (size_t)inflateStream.total_out);
        } else if (result != Z_BUF_ERROR) {
            printf("inflate failed (%d: %s).\n", (int)result, inflateStream.msg);
            return;
        }
    }

    // Display inflated data.
    printf("inflated data: %s\n", output.c_str());

    // End the inflate stream.
    result = inflateEnd(&inflateStream);
    if (result == Z_OK) {
        printf("inflateEnd succeeded.\n");
    } else {
        printf("inflateEnd failed (%d: %s).\n", (int)result, inflateStream.msg);
        return;
    }

    // Display some stats.
    printf("Original:   %zu bytes\n", input.length());
    printf("Compressed: %zu bytes\n", deflatedContent.size());
}

/**
 * This function does stuff with the inflate/deflate functions from zlib.
 *
 * @param[in] environment
 *     This contains variables set through the operating system environment
 *     or the command-line arguments.
 */
void PlayWithGzip(const Environment& environment) {
    printf("\nPlaying with gzip:\n-----------------------------\n");

    // This is the input to the process: the string to deflate and then
    // inflate.
    const std::string input = "Hello, World!";
    // const std::string input = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\"> <html> <head> <meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"> <title>The LLVM Compiler Infrastructure Project</title> <link rel=\"stylesheet\" href=\"/llvm.css\" type=\"text/css\"> </head> <body style=\"margin-left:0;\"> <div class=\"www_title\"> The <strong>LLVM</strong> Compiler Infrastructure </div> <table width=\"100%\" border=0 cellpadding=10> <tr> <td valign=top align=center> <!-- <img src=\"img/DragonSmall.png\" width=\"136\" height=\"136\"> --> <b>Site Map:</b> <div class=\"www_sidebar\"> <a href=\"/\">Overview</a><br> <a href=\"/Features.html\">Features</a><br> <a href=\"/docs/\">Documentation</a><br> <a href=\"/docs/CommandGuide/\">Command Guide</a><br> <a href=\"/docs/FAQ.html\">FAQ</a><br> <a href=\"/pubs/\">Publications</a><br> <a href=\"/ProjectsWithLLVM/\">LLVM&nbsp;Projects</a><br> <a href=\"/OpenProjects.html\">Open&nbsp;Projects</a><br> <a href=\"/Users.html\">LLVM&nbsp;Users</a><br> <a href=\"http://bugs.llvm.org/\">Bug&nbsp;Database</a><br> <a href=\"/Logo.html\">LLVM Logo</a><br> <a href=\"http://blog.llvm.org/\">Blog</a><br> <a href=\"/devmtg/\">Meetings</a><br> <a href=\"/foundation/\">LLVM&nbsp;Foundation</a><br> </div> <br> <b>Download!</b> <div class=\"www_sidebar\"> <span style=\"font-size:smaller\">Download now:</span> <a href=\"/releases/download.html#7.0.0\"><b>LLVM&nbsp;7.0.0</b></a><br> <a href=\"/releases/\">All Releases</a><br> <a href=\"https://apt.llvm.org/\">APT Packages</a><br> <a href=\"/builds/\">Win Installer</a><br> <br> <span style=\"font-size:smaller\">View the open-source</span><br> <a href=\"/releases/7.0.0/LICENSE.TXT\"><b>license</b></a> </div> <br> <b>Search&nbsp;this&nbsp;Site</b> <div class=\"www_sidebar\"> <form action=\"https://www.google.com/search\" method=\"get\"> <div> <input type=\"hidden\" name=\"sitesearch\" value=\"llvm.org\"> <input type=text name=q size=11><br> <input type=submit value=\"Search!\" name=\"submit\"> </div> </form> </div> <br> <b>Useful&nbsp;Links</b> <div class=\"www_sidebar\"> <span style=\"font-size:smaller\">Mailing Lists:</span><br> <a href=\"http://lists.llvm.org/mailman/listinfo/llvm-announce\">LLVM-announce</a><br> <a href=\"http://lists.llvm.org/mailman/listinfo/llvm-dev\">LLVM-dev</a><br> <a href=\"http://lists.llvm.org/mailman/listinfo/llvm-bugs\">LLVM-bugs</a><br> <a href=\"http://lists.llvm.org/mailman/listinfo/llvm-commits\">LLVM-commits</a><br> <a href=\"http://lists.llvm.org/mailman/listinfo/llvm-branch-commits\">LLVM-branch-commits</a><br> <a href=\"http://lists.llvm.org/mailman/listinfo/llvm-testresults\">LLVM-testresults</a> <br><br> <span style=\"font-size:smaller\">IRC Channel:</span><br> <a href=\"/docs/#irc\">irc.oftc.net #llvm</a> <br><br> <span style=\"font-size:smaller\">Dev. Resources:</span><br> <a href=\"/doxygen/\">doxygen</a> <a href=\"/viewvc/llvm-project/\">ViewVC</a><br> <a href=\"http://blog.llvm.org/\">Blog</a> <a href=\"http://bugs.llvm.org/\">Bugzilla</a><br> <a href=\"http://lab.llvm.org:8011/\">Buildbot</a> <a href=\"http://green.lab.llvm.org/green/\">Green Dragon</a> <a href=\"http://lnt.llvm.org/\">LNT</a> <a href=\"/reports/coverage/\">Coverage</a> <a href=\"/reports/scan-build/\">Scan-build</a> <a href=\"http://lab.llvm.org:8080/coverage/coverage-reports/index.html\">llvm-cov</a> </div> <br> <b>Release&nbsp;Emails</b> <div class=\"www_sidebar\"> <span style=\"font-size:smaller\"> 7.0.0: <a href=\"http://lists.llvm.org/pipermail/llvm-announce/2018-September/000080.html\">Sep 2018</a><br> 6.0.1: <a href=\"http://lists.llvm.org/pipermail/llvm-announce/2018-July/000079.html\">July 2018</a><br> 5.0.2: <a href=\"http://lists.llvm.org/pipermail/llvm-announce/2018-May/000078.html\">May 2018</a><br> 6.0.0: <a href=\"http://lists.llvm.org/pipermail/llvm-announce/2018-March/000077.html\">Mar 2018</a><br> 5.0.1: <a href=\"http://lists.llvm.org/pipermail/llvm-announce/2017-December/000076.html\">Dec 2017</a><br> 5.0.0: <a href=\"http://lists.llvm.org/pipermail/llvm-announce/2017-September/000075.html\">Sep 2017</a><br> 4.0.1: <a href=\"http://lists.llvm.org/pipermail/llvm-announce/2017-July/000074.html\">Jul 2017</a><br> 4.0.0: <a href=\"http://lists.llvm.org/pipermail/llvm-announce/2017-March/000073.html\">Mar 2017</a><br> 3.9.1: <a href=\"http://lists.llvm.org/pipermail/llvm-announce/2016-December/000072.html\">Dec 2016</a><br> 3.9.0: <a href=\"http://lists.llvm.org/pipermail/llvm-announce/2016-September/000070.html\">Sep 2016</a><br> 3.8.1: <a href=\"http://lists.llvm.org/pipermail/llvm-announce/2016-July/000068.html\">July 2016</a><br> 3.8.0: <a href=\"http://lists.llvm.org/pipermail/llvm-announce/2016-March/000067.html\">Mar 2016</a><br> 3.7.1: <a href=\"http://lists.llvm.org/pipermail/llvm-announce/2016-January/000066.html\">Jan 2016</a><br> 3.7.0: <a href=\"http://lists.llvm.org/pipermail/llvm-announce/2015-September/000065.html\">Sep 2015</a><br> 3.6.2: <a href=\"http://lists.llvm.org/pipermail/llvm-announce/2015-July/000061.html\">Jul 2015</a><br> 3.6.1: <a href=\"http://lists.llvm.org/pipermail/llvm-announce/2015-May/000059.html\">May 2015</a><br> 3.5.2: <a href=\"http://lists.llvm.org/pipermail/llvm-announce/2015-April/000058.html\">Apr 2015</a><br> 3.6: <a href=\"http://lists.llvm.org/pipermail/llvm-announce/2015-February/000057.html\">Feb 2015</a><br> 3.5.1: <a href=\"http://lists.llvm.org/pipermail/llvm-announce/2015-January/000056.html\">Jan 2015</a><br> 3.5: <a href=\"http://lists.llvm.org/pipermail/llvm-announce/2014-September/000055.html\">Sep 2014</a><br> 3.4.2: <a href=\"http://lists.llvm.org/pipermail/llvm-announce/2014-June/000053.html\">June 2014</a><br> 3.4.1: <a href=\"http://lists.llvm.org/pipermail/llvm-announce/2014-May/000052.html\">May 2014</a><br> 3.4: <a href=\"http://lists.llvm.org/pipermail/llvm-announce/2014-January/000049.html\">Jan 2014</a><br> 3.3: <a href=\"http://lists.llvm.org/pipermail/llvm-announce/2013-June/000046.html\">Jun 2013</a><br> 3.2: <a href=\"http://lists.llvm.org/pipermail/llvm-announce/2012-December/000043.html\">Dec 2012</a><br> 3.1: <a href=\"http://lists.llvm.org/pipermail/llvm-announce/2012-May/000041.html\">May 2012</a><br> 3.0: <a href=\"http://lists.llvm.org/pipermail/llvm-announce/2011-December/000039.html\">Dec 2011</a><br> 2.9: <a href=\"http://lists.llvm.org/pipermail/llvm-announce/2011-April/000037.html\">Apr 2011</a><br> 2.8: <a href=\"http://lists.llvm.org/pipermail/llvm-announce/2010-October/000036.html\">Oct  2010</a><br> 2.7: <a href=\"http://lists.llvm.org/pipermail/llvm-announce/2010-April/000034.html\">Apr 2010</a><br> 2.6: <a href=\"http://lists.llvm.org/pipermail/llvm-announce/2009-October/000033.html\">Oct 2009</a><br> 2.5: <a href=\"http://lists.llvm.org/pipermail/llvm-announce/2009-March/000031.html\">Mar 2009</a><br> 2.4: <a href=\"http://lists.llvm.org/pipermail/llvm-announce/2008-November/000030.html\">Nov 2008</a><br> 2.3: <a href=\"http://lists.llvm.org/pipermail/llvm-announce/2008-June/000027.html\">Jun 2008</a><br> 2.2: <a href=\"http://lists.llvm.org/pipermail/llvm-announce/2008-February/000025.html\">Feb 2008</a><br> 2.1: <a href=\"http://lists.llvm.org/pipermail/llvm-announce/2007-September/000024.html\">Sep 2007</a><br> 2.0: <a href=\"http://lists.llvm.org/pipermail/llvm-announce/2007-May/000023.html\">May 2007</a><br> <a href=\"/OldNews.html\">Older News</a><br> </span> </div> <br> <span style=\"font-size:smaller\">Maintained by the <br> <a href=\"mailto:llvm-admin@lists.llvm.org\">llvm-admin team</a></span> </td> <td valign=top> <table border=\"0\" cellpadding=\"0\" cellspacing=\"0\" width=\"95%\"> <!-- Start of the left bar... --> <tr> <td valign=\"top\" style=\"width:64%\"> <div class=\"www_sectiontitle\">LLVM Overview</div> <div class=\"www_text\"> <p>The LLVM Project is a collection of modular and reusable compiler and toolchain technologies.  Despite its name, LLVM has little to do with traditional virtual machines.  The name \"LLVM\" itself is not an acronym; it is the full name of the project.</p> <p>LLVM began as a <a href=\"pubs/2004-01-30-CGO-LLVM.html\">research project</a> at the <a href=\"http://cs.illinois.edu/\">University of Illinois</a>, with the goal of providing a modern, SSA-based compilation strategy capable of supporting both static and dynamic compilation of arbitrary programming languages.  Since then, LLVM has grown to be an umbrella project consisting of a number of subprojects, many of which are being used in production by a wide variety of <a href=\"Users.html\">commercial and open source</a> projects as well as being widely used in <a href=\"pubs/\">academic research</a>.  Code in the LLVM project is licensed under the <a href=\"docs/DeveloperPolicy.html#license\">\"UIUC\" BSD-Style license</a>.</p> <p>The primary sub-projects of LLVM are:</p> <ol> <li><p>The <b>LLVM Core</b> libraries provide a modern source- and target-independent <a href=\"docs/Passes.html\">optimizer</a>, along with <a href=\"docs/CodeGenerator.html\">code generation support</a> for many popular CPUs (as well as some less common ones!) These libraries are built around a <a href=\"docs/LangRef.html\">well specified</a> code representation known as the LLVM intermediate representation (\"LLVM IR\").  The LLVM Core libraries are <a href=\"docs/\">well documented</a>, and it is particularly easy to invent your own language (or port an existing compiler) to use <a href=\"docs/tutorial/\">LLVM as an optimizer and code generator</a>.</p> </li> <li><p><b><a href=\"http://clang.llvm.org\">Clang</a></b> is an \"LLVM native\" C/C++/Objective-C compiler, which aims to deliver amazingly fast compiles (e.g. about <a href=\"http://clang.llvm.org/features.html#performance\">3x faster than GCC</a> when compiling Objective-C code in a debug configuration), extremely useful <a href=\"http://clang.llvm.org/diagnostics.html\">error and warning messages</a> and to provide a platform"
    // " for building great source level tools.  The <a href=\"http://clang-analyzer.llvm.org/\">Clang Static Analyzer</a> is a tool that automatically finds bugs in your code, and is a great example of the sort of tool that can be built using the Clang frontend as a library to parse C/C++ code.</p></li> <li><p>The <b><a href=\"http://lldb.llvm.org\">LLDB</a></b> project builds on libraries provided by LLVM and Clang to provide a great native debugger. It uses the Clang ASTs and expression parser, LLVM JIT, LLVM disassembler, etc so that it provides an experience that \"just works\".  It is also blazing fast and much more memory efficient than GDB at loading symbols. </p></li> <li><p>The <b><a href=\"http://libcxx.llvm.org\">libc++</a></b> and <b><a href=\"http://libcxxabi.llvm.org\">libc++ ABI</a></b> projects provide a standard conformant and high-performance implementation of the C++ Standard Library, including full support for C++11 and C++14.</p></li> <li><p>The <b><a href=\"http://compiler-rt.llvm.org\">compiler-rt</a></b> project provides highly tuned implementations of the low-level code generator support routines like \"<tt>__fixunsdfdi</tt>\" and other calls generated when a target doesn't have a short sequence of native instructions to implement a core IR operation. It also provides implementations of run-time libraries for dynamic testing tools such as <a href=\"http://clang.llvm.org/docs/AddressSanitizer.html\">AddressSanitizer</a>, <a href=\"http://clang.llvm.org/docs/ThreadSanitizer.html\">ThreadSanitizer</a>, <a href=\"http://clang.llvm.org/docs/MemorySanitizer.html\">MemorySanitizer</a>, and <a href=\"http://clang.llvm.org/docs/DataFlowSanitizer.html\">DataFlowSanitizer</a>. </p></li> <li><p>The <b><a href=\"http://openmp.llvm.org\">OpenMP</a></b> subproject provides an <a href=\"http://openmp.org\">OpenMP</a> runtime for use with the OpenMP implementation in Clang.</p></li> <li><p>The <b><a href=\"http://polly.llvm.org/\">polly</a></b> project implements a suite of cache-locality optimizations as well as auto-parallelism and vectorization using a polyhedral model.</p></li> <li><p>The <b><a href=\"http://libclc.llvm.org/\">libclc</a></b> project aims to implement the OpenCL standard library.</p></li> <li><p>The <b><a href=\"http://klee.llvm.org\">klee</a></b> project implements a \"symbolic virtual machine\" which uses a theorem prover to try to evaluate all dynamic paths through a program in an effort to find bugs and to prove properties of functions.  A major feature of klee is that it can produce a testcase in the event that it detects a bug.</p></li> <li><p>The <b><a href=\"http://safecode.cs.illinois.edu\">SAFECode</a></b> project is a memory safety compiler for C/C++ programs.  It instruments code with run-time checks to detect memory safety errors (e.g., buffer overflows) at run-time.  It can be used to protect software from security attacks and can also be used as a memory safety error debugging tool like Valgrind.</p></li> <li><p>The <b><a href=\"http://lld.llvm.org/\">LLD</a></b> project is a new linker. That is a drop-in replacement for system linkers and runs much faster.</p></li> </ol> <p>In addition to official subprojects of LLVM, there are a broad variety of other projects that <a href=\"/ProjectsWithLLVM/\">use components of LLVM for various tasks</a>.  Through these external projects you can use LLVM to compile Ruby, Python, Haskell, Java, D, PHP, Pure, Lua, and a number of other languages.  A major strength of LLVM is its versatility, flexibility, and reusability, which is why it is being used for such a wide variety of different tasks: everything from doing light-weight JIT compiles of embedded languages like Lua to compiling Fortran code for massive super computers.</p> <p>As much as everything else, LLVM has a broad and friendly community of people who are interested in building great low-level tools.  If you are interested in getting involved, a good first place is to skim the <a href=\"http://blog.llvm.org\">LLVM Blog</a> and to sign up for the <a href=\"http://lists.llvm.org/mailman/listinfo/llvm-dev\">LLVM Developer mailing list</a>.  For information on how to send in a patch, get commit access, and copyright and license topics, please see <a href=\"docs/DeveloperPolicy.html\">the LLVM Developer Policy</a>. </p> </div> </td> <!-- Dividing space between columns --> <td style=\"width:2%\"></td> <!-- Start of the right bar... 359 --> <td valign=\"top\" style=\"width:35%\"> <div class=\"www_sectiontitle\">Latest LLVM Release!</div> <div class=\"www_text\"> <p><b>19 September 2018</b>: LLVM 7.0.0 is now <a href=\"releases/\"><b>available for download</b></a>!  LLVM is publicly available under an open source <a href=\"releases/7.0.0/LICENSE.TXT\">License</a>.  Also, you might want to check out <a href=\"docs/ReleaseNotes.html#whatsnew\"><b>the new features</b></a> in SVN that will appear in the next LLVM release.  If you want them early, <a href=\"releases/\">download LLVM</a> through anonymous SVN.</p> </div> <div class=\"www_sectiontitle\">ACM Software System Award!</div> <div class=\"www_text\"> <p>LLVM has been awarded the <b>2012 ACM Software System Award</b>! This award is given by ACM to <i>one</i> software system worldwide every year. <!-- and is \"<i>Awarded to an institution or individual(s) recognized for developing a software system that has had a lasting influence, reflected in contributions to concepts, in commercial acceptance, or both</i>\".--> LLVM is <a href=\"http://awards.acm.org/software_system/year.cfm\"> in highly distinguished company</a>! Click on any of the individual recipients' names on that page for the detailed citation describing the award. </p> </div> <div class=\"www_sectiontitle\">Upcoming Releases</div> <div class=\"www_text\"> <p><b>LLVM Release Schedule:</b></p> <ul> <li>7.0.1: <ul> <li>To be decided.</li> </ul> </li> </ul> </div> <div class=\"www_sectiontitle\">Developer&nbsp;Meetings</div> <div class=\"www_text\"> <p>Upcoming: <a href=\"/devmtg/2018-10/\">October 17-18, 2018 (Workshop October 16)</a></p> <p>Proceedings from past meetings:</p> <ul> <li><a href=\"/devmtg/2018-04/\">April 16-17, 2018</a></li> <li><a href=\"/devmtg/2017-10/\">October 18-19, 2017</a></li> <li><a href=\"/devmtg/2017-03/\">March 27-28, 2017</a></li> <li><a href=\"/devmtg/2016-11/\">November 3-4, 2016</a></li> <li><a href=\"/devmtg/2016-03/\">March 17-18, 2016</a></li> <li><a href=\"/devmtg/2015-10/\">October 29-30, 2015</a></li> <li><a href=\"/devmtg/2015-04/\">April 13-14, 2015</a></li> <li><a href=\"/devmtg/2014-10/\">October 28-29, 2014</a></li> <li><a href=\"/devmtg/2014-04/\">April 7-8, 2014</a></li> <li><a href=\"/devmtg/2013-11/\">Nov 6-7, 2013</a></li> <li><a href=\"/devmtg/2013-04/\">April 29-30, 2013</a></li> <li><a href=\"/devmtg/2012-11/\">November 7-8, 2012</a></li> <li><a href=\"/devmtg/2012-04-12/\">April 12, 2012</a></li> <li><a href=\"/devmtg/2011-11/\">November 18, 2011</a></li> <li><a href=\"/devmtg/2011-09-16/\">September 2011</a></li> <li><a href=\"/devmtg/2010-11/\">November 2010</a></li> <li><a href=\"/devmtg/2009-10/\">October 2009</a></li> <li><a href=\"/devmtg/2008-08/\">August 2008</a></li> <li><a href=\"/devmtg/2007-05/\">May 2007</a></li> </ul> </div> <br> <!-- End of the right column --> </td> </tr> </table> </td></tr> </table> </body> </html> ";

    // Here's the buffer in which we'll put the deflated data.
    std::vector< uint8_t > deflatedContent;

    // Here's the buffer in which we'll put the inflated data.
    std::vector< uint8_t > inflatedContent(INFLATE_BUFFER_SIZE);

    // This is where we'll build up the inflated data output.
    std::string output;

    // Initialize the deflate stream.
    z_stream deflateStream;
    deflateStream.zalloc = Z_NULL;
    deflateStream.zfree = Z_NULL;
    deflateStream.opaque = Z_NULL;
    if (
        deflateInit2(
            &deflateStream,
            Z_DEFAULT_COMPRESSION,
            Z_DEFLATED,
            16 + MAX_WBITS,
            8,
            Z_DEFAULT_STRATEGY
        ) == Z_OK
    ) {
        printf("deflateInit2 succeeded.\n");
    } else {
        printf("deflateInit2 failed.\n");
        return;
    }

    // Deflate the data.
    deflateStream.next_in = (Bytef*)input.c_str();
    deflateStream.avail_in = (uInt)input.length();
    deflateStream.total_in = 0;
    int result = Z_OK;
    while (result != Z_STREAM_END) {
        size_t totalDeflatedPreviously = deflatedContent.size();
        deflatedContent.resize(totalDeflatedPreviously + DEFLATE_BUFFER_INCREMENT);
        deflateStream.next_out = (Bytef*)deflatedContent.data() + totalDeflatedPreviously;
        deflateStream.avail_out = DEFLATE_BUFFER_INCREMENT;
        deflateStream.total_out = 0;
        result = deflate(&deflateStream, Z_FINISH);
        deflatedContent.resize(totalDeflatedPreviously + (size_t)deflateStream.total_out);
        if (result == Z_OK) {
            printf("deflate produced %zu more bytes.\n", (size_t)deflateStream.total_out);
        } else if (result == Z_STREAM_END) {
            printf("deflate produced last %zu bytes.\n", (size_t)deflateStream.total_out);
        } else if (result != Z_BUF_ERROR) {
            printf("deflate failed (%d: %s).\n", (int)result, deflateStream.msg);
            return;
        }
    }

    // Display deflated data.
    printf("Deflated data:");
    for (size_t i = 0; i < deflatedContent.size(); ++i) {
        if ((i % 16) == 0) {
            printf("\n");
        } else {
            printf(" ");
        }
        printf("%02" PRIX8, deflatedContent[i]);
    }
    printf("\n");

    // Write deflated data to a file, for offline testing.
    DumpFile("test.gz", deflatedContent);

    // End the deflate stream.
    result = deflateEnd(&deflateStream);
    if (result == Z_OK) {
        printf("deflateEnd succeeded.\n");
    } else {
        printf("deflateEnd failed (%d: %s).\n", (int)result, deflateStream.msg);
        return;
    }

    // Initialize the inflate stream.
    z_stream inflateStream;
    inflateStream.zalloc = Z_NULL;
    inflateStream.zfree = Z_NULL;
    inflateStream.opaque = Z_NULL;
    if (
        inflateInit2(
            &inflateStream,
            16 + MAX_WBITS
        ) == Z_OK
    ) {
        printf("inflateInit2 succeeded.\n");
    } else {
        printf("inflateInit2 failed.\n");
        return;
    }

    // inflate the data.
    inflateStream.next_in = (Bytef*)deflatedContent.data();
    inflateStream.avail_in = (uInt)deflatedContent.size();
    inflateStream.total_in = 0;
    result = Z_OK;
    while (result != Z_STREAM_END) {
        size_t totalInflatedPreviously = inflatedContent.size();
        inflateStream.next_out = (Bytef*)inflatedContent.data();
        inflateStream.avail_out = (uInt)inflatedContent.size();
        inflateStream.total_out = 0;
        result = inflate(&inflateStream, Z_FINISH);
        output += std::string(
            inflatedContent.begin(),
            inflatedContent.begin() + (size_t)inflateStream.total_out
        );
        if (result == Z_OK) {
            printf("inflate produced %zu more bytes.\n", (size_t)inflateStream.total_out);
        } else if (result == Z_STREAM_END) {
            printf("inflate produced last %zu bytes.\n", (size_t)inflateStream.total_out);
        } else if (result != Z_BUF_ERROR) {
            printf("inflate failed (%d: %s).\n", (int)result, inflateStream.msg);
            return;
        }
    }

    // Display inflated data.
    printf("inflated data: %s\n", output.c_str());

    // End the inflate stream.
    result = inflateEnd(&inflateStream);
    if (result == Z_OK) {
        printf("inflateEnd succeeded.\n");
    } else {
        printf("inflateEnd failed (%d: %s).\n", (int)result, inflateStream.msg);
        return;
    }

    // Display some stats.
    printf("Original:   %zu bytes\n", input.length());
    printf("Compressed: %zu bytes\n", deflatedContent.size());
}

/**
 * This function is the entrypoint of the program.
 * It just sets up the bot and has it log into Twitch.  At that point, the
 * bot will interact with Twitch using its callbacks.  It registers the
 * SIGINT signal to know when the bot should be shut down.
 *
 * The program is terminated after the SIGINT signal is caught.
 *
 * @param[in] argc
 *     This is the number of command-line arguments given to the program.
 *
 * @param[in] argv
 *     This is the array of command-line arguments given to the program.
 */
int main(int argc, char* argv[]) {
#ifdef _WIN32
    //_crtBreakAlloc = 18;
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif /* _WIN32 */
    Environment environment;
    (void)setbuf(stdout, NULL);
    if (!ProcessCommandLineArguments(argc, argv, environment)) {
        PrintUsageInformation();
        return EXIT_FAILURE;
    }
    PlayWithInflateDeflate(environment);
    PlayWithGzip(environment);
    return EXIT_SUCCESS;
}
