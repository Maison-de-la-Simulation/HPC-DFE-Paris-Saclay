// Open links to other sites in a new tab. Links inside the course site
// keep opening in the same tab.
// document$ is provided by Material and fires on every page load, including
// the pages loaded by instant navigation (navigation.instant).
document$.subscribe(function () {
  document.querySelectorAll(".md-content a[href]").forEach(function (link) {
    if (link.hostname && link.hostname !== window.location.hostname) {
      link.target = "_blank";
      link.rel = "noopener";
    }
  });
});
