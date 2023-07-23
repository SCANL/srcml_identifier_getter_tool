#ifndef CLONE_GET_HPP
#define CLONE_GET_HPP

#include <iostream>
#include <string>
#include <vector>
#include <git2.h>

void clone_git_repository(const std::string& link, const std::string& destination_folder) {
    git_repository *repo = nullptr;
    git_clone_options clone_opts = GIT_CLONE_OPTIONS_INIT;

    // Set the repository path
    std::string repo_path = destination_folder + "/" + link.substr(link.find_last_of('/') + 1);
    clone_opts.checkout_opts.checkout_strategy = GIT_CHECKOUT_SAFE;

    std::cout<<"Cloning: "<<repo_path<<std::endl;
    // Clone the repository
    int error = git_clone(&repo, link.c_str(), repo_path.c_str(), &clone_opts);
    if (error == 0) {
        std::cout << "Successfully cloned " << link << std::endl;
    } else {
        const git_error *e = giterr_last();
        std::cout << "Failed to clone " << link << ": " << e->message << std::endl;
    }

    // Cleanup
    if (repo)
        git_repository_free(repo);
}

int main() {
    git_libgit2_init();

    std::vector<std::string> git_links = {
        "https://github.com/s3fs-fuse/s3fs-fuse.git",
        "https://github.com/wireshark/wireshark.git",
        "https://github.com/igniterealtime/Openfire.git",
        "https://github.com/google/guava.git",
        "https://github.com/irungentoo/toxcore.git",
        "https://github.com/BVLC/caffe.git",
        "https://github.com/dropwizard/metrics.git",
        "https://github.com/facebook/proxygen.git",
        "https://github.com/liuliu/ccv.git",
        "https://github.com/deeplearning4j/deeplearning4j.git",
        "https://github.com/ipkn/crow.git",
        "https://github.com/toggl-open-source/toggldesktop.git",
        "https://github.com/SonarSource/sonarqube.git",
        "https://github.com/mgba-emu/mgba.git",
        "https://github.com/openvswitch/ovs.git",
        "https://github.com/immutables/immutables.git",
        "https://github.com/vgvassilev/cling.git",
        "https://github.com/weechat/weechat.git",
        "https://github.com/cglib/cglib.git",
        "https://github.com/panda3d/panda3d.git",
        "https://github.com/igraph/rigraph.git",
        "https://github.com/meta-toolkit/meta.git",
        "https://github.com/igniterealtime/Spark.git",
        "https://github.com/ropensci/git2r.git",
        "https://github.com/freeminer/freeminer.git",
        "https://github.com/igniterealtime/Smack.git",
        "https://github.com/naemon/naemon-core.git",
        "https://github.com/fakeNetflix/facebook-repo-ds2.git",
        "https://github.com/HubSpot/Singularity.git",
        "https://github.com/Juniper/libxo.git",
    };

    std::string destination_folder = "cloned_repositories";

    for (const auto& link : git_links) {
        clone_git_repository(link, destination_folder);
    }

    git_libgit2_shutdown();
    return 0;
}
#endif
